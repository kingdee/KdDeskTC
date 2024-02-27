use clap::App;
mod common;
mod relay_server;
// use flexi_logger::*;
use hbb_common::{config::RELAY_PORT, ResultType};
use relay_server::*;
mod version;

use log;
use log4rs;

fn main() -> ResultType<()> {
    // let _logger = Logger::try_with_env_or_str("info")?
    //     .log_to_stdout()
    //     .format(opt_format)
    //     .write_mode(WriteMode::Async)
    //     .start()?;

    if let Err(_) = log4rs::init_file("hbbr_log4rs.yaml", Default::default()) {
        println!("[hbbr_log4rs.yaml] log config file is missing!");
        return Ok(());
    }

    let args = format!(
        "-p, --port=[NUMBER(default={})] 'Sets the listening port'
        -k, --key=[KEY] 'Only allow the client with the same key'
        ",
        RELAY_PORT,
    );
    let matches = App::new("hbbr")
        .version(version::VERSION)
        .author("Purslane Ltd. <info@rustdesk.com>")
        .about("RustDesk Relay Server")
        .args_from_usage(&args)
        .get_matches();
    if let Ok(v) = ini::Ini::load_from_file(".env") {
        if let Some(section) = v.section(None::<String>) {
            section.iter().for_each(|(k, v)| std::env::set_var(k, v));
        }
    }
    start(
        matches.value_of("port").unwrap_or(&RELAY_PORT.to_string()),
        matches.value_of("key").unwrap_or(""),
    )?;
    Ok(())
}
