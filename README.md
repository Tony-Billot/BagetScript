# 🥖 BagetScript (.bgt)

BagetScript is a simple interpreted programming language written in C++.  
It is designed to make learning programming and algorithms easier through a clean, French-inspired syntax.

The goal is not performance or production use, but education: helping beginners focus on logic rather than complex syntax.

---

## ✨ Philosophy

BagetScript aims to make code more readable for French speakers by using natural language keywords:

- `si` instead of `if`
- `sinon` instead of `else`
- `tantque` instead of `while`
- `afficher` instead of `print`
- `nombre`, `texte`, `booléen` for types

---

## 📜 Example

```bgt
nombre a = 5;
nombre b = 8;

afficher(a + b);

// Sortie : 13
```

## 🧩 VS Code extension
A VS Code extension is included in the `vscode-extension` folder. It recognizes `.bgt` files, adds syntax highlighting and provides snippets (type `afficher`, `nombre` or `texte`, then press Tab).

### Installation

1. Download or clone this repository.
2. Copy the `vscode-extension` folder into your VS Code extensions folder, and rename it `bagetscript`:
   - **Windows:** `C:\Users\<your name>\.vscode\extensions\`
   - **macOS / Linux:** `~/.vscode/extensions/`
3. Restart VS Code.

Open any `.bgt` file: "BagetScript" should appear in the bottom-right corner of VS Code.
