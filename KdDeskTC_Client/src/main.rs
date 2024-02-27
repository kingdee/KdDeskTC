// Specify the Windows subsystem to eliminate console window.
// Requires Rust 1.18.
// modified by fantacy.
#![windows_subsystem = "windows"]

use hbb_common::log;
use rustdesk::*;
// added by fantacy.
use std::path::PathBuf;
// use std::process;
use std::env;
use std::io;
//use directories_next::ProjectDirs;

/*use hbb_common::{
    allow_err,
    config::{self, Config, LocalConfig, PeerConfig, APP_NAME, ICON},
};*/
#[cfg(any(target_os = "android", target_os = "ios"))]
fn main() {
    common::test_rendezvous_server();
    common::test_nat_type();
    #[cfg(target_os = "android")]
    crate::common::check_software_update();
    mobile::Session::start("");
}
fn inner_main() -> io::Result<PathBuf> {
    //let path2 = env::current_dir()?;
    //println!("working directory -> get:{}.", path2.display());
    let mut dir = env::current_exe()?;
    println!("exe -> {}.", dir.display());
    dir.pop();
    Ok(dir)
}



// #[cfg(windows)]
// pub fn decode_wide(mut wide_c_string: &[u16]) -> bool {
//     use std::ffi::{OsStr, OsString};
//     use std::os::windows::ffi::OsStringExt;
//     if let Some(null_pos) = wide_c_string.iter().position(|c| *c == 0) {
//         wide_c_string = &wide_c_string[..null_pos];
//         return true;
//     }
//     return false;
// }

#[cfg(windows)]
fn create_mutex_for_app() -> bool
{
    //use winapi::shared::minwindef::DWORD;
    use std::ffi::{OsStr, OsString};
    use winapi::um::winuser::{GetDesktopWindow, GetPropW, GetWindow, GW_CHILD, GW_HWNDNEXT, IsIconic, IsWindow, SetForegroundWindow, ShowWindow, SW_RESTORE, SW_SHOWNORMAL};
    use winapi::um::synchapi::CreateMutexW;
    use winapi::um::errhandlingapi::GetLastError;
    use std::os::windows::ffi::OsStrExt;
    let prop_name : Vec<u16> = OsStr::new(hbb_common::APP_MUTEX_NAME).encode_wide().chain(std::iter::once(0)).collect();
    let wide_mutex_name : Vec<u16> = OsStr::new(hbb_common::APP_MUTEX_NAME).encode_wide().chain(std::iter::once(0)).collect();
    let _ret = unsafe { CreateMutexW(std::ptr::null_mut(), winapi::shared::minwindef::FALSE, wide_mutex_name.as_ptr()) };
    let err_code = unsafe { GetLastError() };
    if err_code == winapi::shared::winerror::ERROR_ALREADY_EXISTS {
        //log::info!("CreateMutexW ERROR_ALREADY_EXISTS");
        let mut h_previous = unsafe { GetWindow( GetDesktopWindow(), GW_CHILD ) };
        //log::info!("GetWindow {}", hPrevious as i32);
        let mut max_try = 0;
        while  unsafe { IsWindow(h_previous) } != 0  {
            //log::info!("IsWindow {} max_try {} ", hPrevious as i32, max_try);
            if unsafe { GetPropW(h_previous, prop_name.as_ptr()) } != std::ptr::null_mut()  {
                //log::info!("GetPropW {} success", hPrevious as i32);
                if unsafe { IsIconic(h_previous) } != 0 {
                    unsafe { ShowWindow(h_previous, SW_RESTORE) };
                }
                unsafe { SetForegroundWindow(h_previous) };
                unsafe { ShowWindow(h_previous, SW_SHOWNORMAL) };
                break;
            }
            max_try = max_try + 1;
            if max_try > 2000 {
                break;
            }
            h_previous = unsafe { GetWindow(h_previous, GW_HWNDNEXT) };
        }
        return false;
    }
    return true;
}

#[cfg(windows)]
fn chk_rmt_wnd(v: String, v2: String) -> bool
{
    use std::ffi::{OsStr, OsString};
    use winapi::um::winuser::{GetDesktopWindow, GetPropW, GetWindow, GW_CHILD, GW_HWNDNEXT, IsIconic, IsWindow, SetForegroundWindow, ShowWindow, SW_RESTORE, SW_SHOWNORMAL};
    use winapi::um::synchapi::CreateMutexW;
    use winapi::um::errhandlingapi::GetLastError;
    use std::os::windows::ffi::OsStrExt;
    let prop_name : Vec<u16> = OsStr::new(format!("kddesktc{}-{}", v.to_string(), v2.to_string()).as_str()).encode_wide().chain(std::iter::once(0)).collect();
   
    let mut h_previous = unsafe { GetWindow( GetDesktopWindow(), GW_CHILD ) };
    let mut max_try = 0;
    while  unsafe { IsWindow(h_previous) } != 0  {
        if unsafe { GetPropW(h_previous, prop_name.as_ptr()) } != std::ptr::null_mut()  {
            if unsafe { IsIconic(h_previous) } != 0 {
                unsafe { ShowWindow(h_previous, SW_RESTORE) };
            }
            unsafe { SetForegroundWindow(h_previous) };
            unsafe { ShowWindow(h_previous, SW_SHOWNORMAL) };
            //break;
            return true;
        }
        max_try = max_try + 1;
        if max_try > 2000 {
            break;
        }
        h_previous = unsafe { GetWindow(h_previous, GW_HWNDNEXT) };
    }
    return false;
}

#[cfg(not(any(target_os = "android", target_os = "ios", feature = "cli")))]
fn main() {
    // added by fantacy.
    //let wkdir = std::path::Path::new("C:\\Program Files (x86)\\KdDeskTC");
    //std::env::set_current_dir(&wkdir);
    //let exe = std::env::current_exe()?;
    //let dir = exe.parent();
    //std::env::set_current_dir(&dir);
    //println!("working directory -> {}", exe.display());
    let path = inner_main().expect("Couldn't");
    // let exe = format!("{}\\KdDeskTC.exe", path.display());
    std::env::set_current_dir(&path).ok();
    println!("working directory -> set:{}", path.display());
    println!("working directory -> get:{}", std::env::current_dir().map(|c| c.display().to_string()).unwrap_or("".to_owned()));
    // https://docs.rs/flexi_logger/latest/flexi_logger/error_info/index.html#write
    let mut _async_logger_holder: Option<flexi_logger::LoggerHandle> = None;
    let mut args: Vec<String> = std::env::args().skip(1).collect();
    if args.len() > 0 && args[0] == "--version" {
        println!("{}", crate::VERSION);
        return;
    }
    // added by fantacy.
    if args.is_empty() {
        //platform::start_svc();
        //std::thread::sleep(std::time::Duration::from_millis(2000));
        //std::process::Command::new(&exe).arg("--start").spawn();
        //std::thread::spawn(|| platform::start_svc());

        #[cfg(windows)]
        {
            std::thread::spawn(move || platform::start_svc());
        }

        #[cfg(not(windows))]
        {
            std::thread::spawn(move || start_server(false, false));
        }
    }
    #[cfg(not(feature = "inline"))]
    {
        use hbb_common::env_logger::*;
        init_from_env(Env::default().filter_or(DEFAULT_FILTER_ENV, "debug"));
    }
    #[cfg(feature = "inline")]
    {
        //println!("inline feature.");
        let mut path2 = hbb_common::config::Config::log_path();
        if args.len() > 0 && args[0].starts_with("--") {
            let name = args[0].replace("--", "");
            if !name.is_empty() {
                path2.push(name);
            }
        }
        use flexi_logger::*;
        if let Ok(x) = Logger::try_with_env_or_str("info") {
            println!("file log path:{}.", path2.display());
            _async_logger_holder = x
                .log_to_file(FileSpec::default().directory(path2))
                .write_mode(WriteMode::Async)
                .format(opt_format)
                .rotate(
                    Criterion::Age(Age::Day),
                    Naming::Timestamps,
                    Cleanup::KeepLogFiles(6),
                )
                .start()
                .ok();
        }
    }
    if args.is_empty() {
        // modified by fantacy.
        //std::thread::spawn(move || start_server(false, false));

        #[cfg(windows)]
        {
            // 禁止开启多个应用
            //if !create_mutex_for_app() {
                return;
            //}
        }
        //let mut path2 = hbb_common::config::Config::path("");
        //println!("config path:{}.", path2.display());
        //let mut cfgpath = hbb_common::config::Config::get_home();
        //println!("config path:{}.", cfgpath.display());
        //let mut sys_cfgpath = hbb_common::config::Config::path("").display().to_string();
        //sys_cfgpath.truncate(2);
        //sys_cfgpath.push_str("\\Windows\\ServiceProfiles\\LocalService\\AppData\\Roaming\\KdDeskTC\\config");
        //println!("sys_cfgpath:{}.", sys_cfgpath);
    } else {
        #[cfg(windows)]
        {
            if args[0] == "--uninstall" {
                // added by fantacy.
                //std::thread::spawn(move || platform::start_svc());
                //std::thread::sleep(std::time::Duration::from_millis(3000));
                //let mut password = "".to_owned();
                //ipc::set_password_ex(password);
                //ipc::set_password(hbb_common::config::Config::get_auto_password());
                //hbb_common::allow_err!(ipc::set_password("".to_owned()));
                //hbb_common::config::Config::load_::<Config>("")
                //let mut password = hbb_common::config::Config::get_auto_password();
                //hbb_common::config::Config::set_password(&password);
                if let Err(err) = platform::uninstall_me() {
                    log::error!("Failed to uninstall: {}", err);
                }
                return;
            } else if args[0] == "--update" {
                hbb_common::allow_err!(platform::update_me());
                return;
            } else if args[0] == "--reinstall" {
                hbb_common::allow_err!(platform::uninstall_me());
                hbb_common::allow_err!(platform::install_me("desktopicon startmenu",));
                return;
            }
            // added by fantacy.
            /*else if args[0] == "--start" {
                if let Err(err) = platform::start_svc() {
                    log::error!("Failed to start service");
                }
                return;
            }*/
        }
        if args[0] == "--remove" {
            if args.len() == 2 {
                // sleep a while so that process of removed exe exit
                std::thread::sleep(std::time::Duration::from_secs(1));
                std::fs::remove_file(&args[1]).ok();
                return;
            }
        } else if args[0] == "--service" {
            log::info!("start --service");
            // added by fantacy.
            //let mut flagex = ipc::get_flagex();
            //if flagex == "1" {
            std::thread::spawn(move || {
                let mut cntr = 0;
                loop {
                    std::thread::sleep(std::time::Duration::from_millis(60*1000));
                    let mut spwdupd = ipc::get_pwdupd();
                    if spwdupd == "0" || spwdupd == "1" {
                        cntr = 0;
                        continue;
                    } else if spwdupd == "2" {
                        cntr+=1;
                        if cntr < 10 {
                            continue;
                        }
                        cntr = 0;
                    } else if spwdupd == "3" {
                        cntr+=1;
                        if cntr < 30 {
                            continue;
                        }
                        cntr = 0;
                    } else if spwdupd == "4" {
                        cntr+=1;
                        if cntr < 1*60 {
                            continue;
                        }
                        cntr = 0;
                    } else if spwdupd == "5" {
                        cntr+=1;
                        if cntr < 3*60 {
                            continue;
                        }
                        cntr = 0;
                    } else if spwdupd == "6" {
                        cntr+=1;
                        if cntr < 4*60 {
                            continue;
                        }
                        cntr = 0;
                    } else if spwdupd == "7" {
                        cntr+=1;
                        if cntr < 24*60 {
                            continue;
                        }
                        cntr = 0;
                    } else {
                        continue;
                    }
                    ipc::set_password(hbb_common::config::Config::get_auto_password());
                }
            });
            start_os_service();
            //}
            //let mut password = hbb_common::config::Config::get_auto_password();
            //hbb_common::config::Config::set_password(&password);
            return;
        } else if args[0] == "--server" {
            log::info!("====main start --server====");
            #[cfg(not(target_os = "macos"))]
            {
                log::info!("====start start_server not macos====");
                start_server(true, true);
                return;
            }
            #[cfg(target_os = "macos")]
            {
                log::info!("====start start_server macos====");
                std::thread::spawn(move || start_server(true, true));
            }
        } else if args[0] == "--import-config" {
            if args.len() == 2 {
                hbb_common::config::Config::import(&args[1]);
            }
            return;
        } else if args[0] == "--password" {
            if args.len() == 2 {
                ipc::set_password(args[1].to_owned()).unwrap();
            }
            return;
        } else if args[0] == "--get-cfg" {
            let mut file = std::fs::File::create("result.ini").unwrap();
            use std::io::prelude::*;
            let mut sid = ipc::get_id();
            let mut spwd = ipc::get_password();
            let mut scstdir = ipc::get_cstdir();
            let mut spwdupd = ipc::get_pwdupd();
            let mut sidle = ipc::get_idle();
            file.write_all(format!("[result]\r\nid={}\r\npwd={}\r\ncstdir={}\r\npwdupd={}\r\nidle={}\r\n", sid.to_string(), spwd.to_string(), scstdir.to_string(), spwdupd.to_string(), sidle.to_string()).as_bytes());
            file.sync_all();
            hbb_common::config::PeerConfig::peers();
            return;
        } else if args[0] == "--upd-pwd" {
            ipc::set_password(hbb_common::config::Config::get_auto_password());
            let mut file = std::fs::File::create("result2.ini").unwrap();
            use std::io::prelude::*;
            let mut spwd = ipc::get_password();
            file.write_all(format!("[result]\r\npwd={}\r\n", spwd.to_string()).as_bytes());
            file.sync_all();
            return;
        } else if args[0] == "--set-cstdir" {
            if args.len() == 2 {
                ipc::set_cstdir(args[1].to_owned()).unwrap();
            }
            return;
        } else if args[0] == "--set-pwdupd" {
            if args.len() == 2 {
                ipc::set_pwdupd(args[1].to_owned()).unwrap();
            }
            return;
        } else if args[0] == "--set-idle" {
            if args.len() == 2 {
                ipc::set_idle(args[1].to_owned()).unwrap();
            }
            return;
        } else if args[0] == "--set-flagex" {
            if args.len() == 2 {
                ipc::set_flagex(args[1].to_owned()).unwrap();
                if args[1] == "0" {
                    std::thread::sleep(std::time::Duration::from_millis(2*1000));
                    platform::stop_svc().ok();
                }
            }
            return;
        } else if args[0] == "--set-uid" {
            if args.len() == 2 {
                ipc::set_uid(args[1].to_owned()).unwrap();
            }
            return;
        } else if args[0] == "--get-updurl" {
            crate::common::check_software_update_ex();
            return;
        } else if args[0] == "--get-idlst" {
            hbb_common::config::PeerConfig::peers();
            return;
        } else if args[0] == "--del-rid" {
            if args.len() == 2 {
            hbb_common::config::PeerConfig::remove(&args[1].to_owned());
            //hbb_common::config::PeerConfig::peers();
            }
            return;
        } else if args[0] == "--reset" {
            ipc::set_password(hbb_common::config::Config::get_auto_password());
            ipc::set_pwdupd("1".to_owned()).unwrap();
            ipc::set_idle("1".to_owned()).unwrap();
            return;
        } else if (args[0] == "--connect" || args[0] == "--file-transfer") && args.len() > 1 {
            if args.len() == 2 {
                if chk_rmt_wnd(args[0].to_owned(), args[1].to_owned()) {
                    return;
                }
            }
        }
    }
    log::info!("====start ui::start====");
    ui::start(&mut args[..]);
    // added by fantacy.
    if args.is_empty() {
        platform::stop_svc().ok();
    }
}

#[cfg(feature = "cli")]
fn main() {
    use clap::App;
    let args = format!(
        "-p, --port-forward=[PORT-FORWARD-OPTIONS] 'Format: remote-id:local-port:remote-port[:remote-host]'
       -s, --server... 'Start server'",
    );
    let matches = App::new("rustdesk")
        .version(crate::VERSION)
        .author("CarrieZ Studio<info@rustdesk.com>")
        .about("RustDesk command line tool")
        .args_from_usage(&args)
        .get_matches();
    use hbb_common::env_logger::*;
    init_from_env(Env::default().filter_or(DEFAULT_FILTER_ENV, "info"));
    if let Some(p) = matches.value_of("port-forward") {
        let options: Vec<String> = p.split(":").map(|x| x.to_owned()).collect();
        if options.len() < 3 {
            log::error!("Wrong port-forward options");
            return;
        }
        let mut port = 0;
        if let Ok(v) = options[1].parse::<i32>() {
            port = v;
        } else {
            log::error!("Wrong local-port");
            return;
        }
        let mut remote_port = 0;
        if let Ok(v) = options[2].parse::<i32>() {
            remote_port = v;
        } else {
            log::error!("Wrong remote-port");
            return;
        }
        let mut remote_host = "localhost".to_owned();
        if options.len() > 3 {
            remote_host = options[3].clone();
        }
        cli::start_one_port_forward(options[0].clone(), port, remote_host, remote_port);
    }
}
