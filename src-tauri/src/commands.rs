use std::fs;

#[tauri::command]
pub fn read_file(path: String) -> Result<String, String> {
    fs::read_to_string(path).map_err(|error| format!("读取文件失败: {error}"))
}

#[tauri::command]
pub fn write_file(path: String, content: String) -> Result<(), String> {
    fs::write(path, content).map_err(|error| format!("保存文件失败: {error}"))
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn read_file_reports_missing_file() {
        let result = read_file("/tmp/md-editor-missing-file.md".to_string());
        assert!(result.is_err());
    }
}
