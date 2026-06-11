mod commands;

pub fn run() {
    tauri::Builder::default()
        .invoke_handler(tauri::generate_handler![
            commands::read_file,
            commands::write_file,
        ])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
