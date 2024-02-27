use crate::common::*;
use crate::database;
use hbb_common::{
    log,
    rendezvous_proto::*,
    tokio::sync::{Mutex, RwLock},
    ResultType,
};
use serde_derive::{Deserialize, Serialize};
use std::{collections::HashMap, collections::HashSet, net::SocketAddr, sync::Arc, time::Instant};

lazy_static::lazy_static! {
    pub(crate) static ref IP_BLOCKER: Mutex<HashMap<String, ((u32, Instant), (HashSet<String>, Instant))>> = Default::default();
    pub(crate) static ref USER_STATUS: RwLock<HashMap<Vec<u8>, Arc<(Option<Vec<u8>>, bool)>>> = Default::default();
    pub(crate) static ref IP_CHANGES: Mutex<HashMap<String, (Instant, HashMap<String, i32>)>> = Default::default();
}
pub static IP_CHANGE_DUR: u64 = 180;
pub static IP_CHANGE_DUR_X2: u64 = IP_CHANGE_DUR * 2;
pub static DAY_SECONDS: u64 = 3600 * 24;
pub static IP_BLOCK_DUR: u64 = 60;

#[derive(Debug, Default, Serialize, Deserialize, Clone)]
pub(crate) struct PeerInfo {
    #[serde(default)]
    pub(crate) ip: String,
}

#[derive(Clone, Debug)]
pub(crate) struct Peer {
    pub(crate) socket_addr: SocketAddr,
    pub(crate) last_reg_time: Instant,
    pub(crate) guid: Vec<u8>,
    pub(crate) uuid: Vec<u8>,
    pub(crate) pk: Vec<u8>,
    pub(crate) user: Option<Vec<u8>>,
    pub(crate) info: PeerInfo,
    pub(crate) disabled: bool,
    pub(crate) reg_pk: (u32, Instant), // how often register_pk
}

impl Default for Peer {
    fn default() -> Self {
        Self {
            socket_addr: "0.0.0.0:0".parse().unwrap(),
            last_reg_time: get_expired_time(),
            guid: Vec::new(),
            uuid: Vec::new(),
            pk: Vec::new(),
            info: Default::default(),
            user: None,
            disabled: false,
            reg_pk: (0, get_expired_time()),
        }
    }
}

pub(crate) type LockPeer = Arc<RwLock<Peer>>;

#[derive(Clone)]
pub(crate) struct PeerMap {
    map: Arc<RwLock<HashMap<String, LockPeer>>>,
    pub(crate) db: database::Database,
}

impl PeerMap {
    pub(crate) async fn new() -> ResultType<Self> {
        let db = std::env::var("DB_URL").unwrap_or({
            #[allow(unused_mut)]
            let mut db = "db_v2.sqlite3".to_owned();
            #[cfg(all(windows, not(debug_assertions)))]
            {
                if let Some(path) = hbb_common::config::Config::icon_path().parent() {
                    db = format!("{}\\{}", path.to_str().unwrap_or("."), db);
                }
            }
            #[cfg(not(windows))]
            {
                db = format!("./{}", db);
            }
            db
        });
        log::info!("DB_URL={}", db);
        let pm = Self {
            map: Default::default(),
            db: database::Database::new(&db).await?,
        };
        Ok(pm)
    }

    #[inline]
    pub(crate) async fn update_pk(
        &mut self,
        id: String,
        peer: LockPeer,
        addr: SocketAddr,
        uuid: Vec<u8>,
        pk: Vec<u8>,
        ip: String,
    ) -> register_pk_response::Result {
        log::info!("update_pk {} {:?} {:?} {:?}", id, addr, uuid, pk);
        let (info_str, guid) = {
            let mut w = peer.write().await;
            w.socket_addr = addr;
            w.uuid = uuid.clone();
            w.pk = pk.clone();
            w.last_reg_time = Instant::now();
            w.info.ip = ip;
            (
                serde_json::to_string(&w.info).unwrap_or_default(),
                w.guid.clone(),
            )
        };
        if guid.is_empty() {
            match self.db.insert_peer(&id, &uuid, &pk, &info_str).await {
                Err(err) => {
                    log::error!("db.insert_peer failed: {}", err);
                    return register_pk_response::Result::SERVER_ERROR;
                }
                Ok(guid) => {
                    peer.write().await.guid = guid;
                }
            }
        } else {
            if let Err(err) = self.db.update_pk(&guid, &id, &pk, &info_str).await {
                log::error!("db.update_pk failed: {}", err);
                return register_pk_response::Result::SERVER_ERROR;
            }
            log::info!("pk updated instead of insert");
        }
        register_pk_response::Result::OK
    }

    #[inline]
    pub(crate) async fn get(&self, id: &str) -> Option<LockPeer> {
        let p = self.map.read().await.get(id).map(|x| x.clone());
        if p.is_some() {
            return p;
        } else {
            if let Ok(Some(v)) = self.db.get_peer(id).await {
                let peer = Peer {
                    guid: v.guid,
                    uuid: v.uuid,
                    pk: v.pk,
                    user: v.user,
                    info: serde_json::from_str::<PeerInfo>(&v.info).unwrap_or_default(),
                    disabled: v.status == Some(0),
                    ..Default::default()
                };
                let peer = Arc::new(RwLock::new(peer));
                self.map.write().await.insert(id.to_owned(), peer.clone());
                return Some(peer);
            }
        }
        None
    }

    #[inline]
    pub(crate) async fn get_or(&self, id: &str) -> LockPeer {
        if let Some(p) = self.get(id).await {
            return p;
        }
        let mut w = self.map.write().await;
        if let Some(p) = w.get(id) {
            return p.clone();
        }
        let tmp = LockPeer::default();
        w.insert(id.to_owned(), tmp.clone());
        tmp
    }

    #[inline]
    pub(crate) async fn get_in_memory(&self, id: &str) -> Option<LockPeer> {
        self.map.read().await.get(id).map(|x| x.clone())
    }

    #[inline]
    pub(crate) async fn is_in_memory(&self, id: &str) -> bool {
        self.map.read().await.contains_key(id)
    }

    #[inline]
    pub(crate) async fn remove(&self, id: &str) {
        self.map.write().await.remove(id);
    }
}
