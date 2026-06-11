const { invoke } = window.__TAURI__.tauri;

const editorFrame = document.querySelector("#editor-frame");
const openButton = document.querySelector("#open-file");
const filePicker = document.querySelector("#file-picker");
const saveButton = document.querySelector("#save-file");
const currentFileLabel = document.querySelector("#current-file");
const documentTitle = document.querySelector("#document-title");
const saveState = document.querySelector("#save-state");
const modeTabs = document.querySelectorAll(".mode-tab");

let currentPath = "";
let currentContent = "# 欢迎使用 md-editor\n\n这是 Rust + Tauri 版本的第一个原型。\n\n- 侧边栏可以横向拖拽\n- 编辑区复用原有 WYSIWYG Markdown 引擎\n- 保存由 Rust 后端写入你指定的路径\n";
let isDirty = false;
let editorReady = false;

function fileNameFromPath(path) {
  return path.split(/[\\/]/).filter(Boolean).pop() || "Untitled";
}

function setStatus(message) {
  saveState.textContent = message;
}

function updateDocumentChrome() {
  const title = currentPath ? fileNameFromPath(currentPath) : "Untitled";
  documentTitle.textContent = isDirty ? `${title} *` : title;
  currentFileLabel.textContent = currentPath || "未选择文件";
}

function postToEditor(message) {
  if (!editorFrame.contentWindow) return;
  editorFrame.contentWindow.postMessage(message, "*");
}

function setEditorContent(content) {
  currentContent = content;
  if (!editorReady) return;
  postToEditor({ type: "setMarkdown", content });
}

async function openPickedFile(file) {
  if (!file) return;

  try {
    const content = await file.text();
    currentPath = file.name;
    isDirty = false;
    setEditorContent(content);
    updateDocumentChrome();
    setStatus("已打开");
  } catch (error) {
    setStatus(`打开失败: ${String(error)}`);
  }
}

function openFile() {
  filePicker.click();
}

function promptSavePath() {
  const suggestedName = currentPath && !currentPath.includes("/") ? currentPath : "untitled.md";
  return window.prompt("输入保存路径", suggestedName) || "";
}

async function saveFile() {
  const path = currentPath.includes("/") ? currentPath : promptSavePath();
  if (!path) return;

  try {
    await invoke("write_file", { path, content: currentContent });
    currentPath = path;
    isDirty = false;
    updateDocumentChrome();
    setStatus("已保存");
  } catch (error) {
    setStatus(String(error));
  }
}

function setMarkdownMode(mode) {
  modeTabs.forEach(tab => {
    tab.classList.toggle("active", tab.dataset.mode === mode);
  });
  postToEditor({ type: "setMarkdownMode", mode });
}

window.addEventListener("message", event => {
  const data = event.data || {};
  if (data.type !== "contentChanged") return;
  currentContent = data.content || "";
  isDirty = true;
  updateDocumentChrome();
  setStatus("未保存");
});

editorFrame.addEventListener("load", () => {
  editorReady = true;
  setEditorContent(currentContent);
  updateDocumentChrome();
});

openButton.addEventListener("click", openFile);
filePicker.addEventListener("change", () => {
  openPickedFile(filePicker.files?.[0]);
  filePicker.value = "";
});
saveButton.addEventListener("click", saveFile);

modeTabs.forEach(tab => {
  tab.addEventListener("click", () => setMarkdownMode(tab.dataset.mode));
});

window.addEventListener("keydown", event => {
  const key = event.key.toLowerCase();
  if (!(event.ctrlKey || event.metaKey)) return;

  if (key === "o") {
    event.preventDefault();
    openFile();
  }

  if (key === "s") {
    event.preventDefault();
    saveFile();
  }
});

updateDocumentChrome();
