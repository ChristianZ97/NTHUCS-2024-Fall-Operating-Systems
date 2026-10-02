# EdSim51 Quick Start

Bundled course-era **EdSim51DI 2.1.38**, developed by James Rogers. The simulator and its companion library are ready to launch; Java is required. This is third-party software, not a student implementation. See [THIRD_PARTY.md](THIRD_PARTY.md).

## 開啟模擬器

macOS：安裝 Java 後，雙擊 `Open EdSim51.command`，或從 repo 根目錄執行：

```sh
brew install openjdk sdcc
./tools/edsim51/launch.sh
```

啟動器會尋找 `JAVA_HOME`、Apple Silicon / Intel Homebrew 的 OpenJDK，以及 PATH 中的 Java。不需要修改 shell 設定。

Linux：安裝 Java runtime（編譯作業另需 SDCC），執行 `./tools/edsim51/launch.sh`。

Windows：安裝 Java 並設好 PATH 或 JAVA_HOME，雙擊 `Open EdSim51.bat`。

檢查環境而不開視窗：

```sh
./tools/edsim51/launch.sh --check
```

## 載入 OS 作業

1. 在 repo 根目錄執行 `make -C assignments/checkpoint01`。
2. 開啟 EdSim51，按 **Load** 選擇 `assignments/checkpoint01/testcoop.hex`。
3. 使用 **Step** 逐步執行或 **Run** 連續執行，觀察暫存器、記憶體與周邊。
4. 換作業時先停止並重設，再載入對應 HEX。

| Checkpoint | 編譯產物 |
| --- | --- |
| 01 | `testcoop.hex` |
| 02 | `testpreempt.hex` |
| 03 | `testpreempt.hex` |
| 04 | `test3threads.hex` |

## 組語範例

使用 Load 載入 `examples/` 下的 `.asm`，再組譯／逐步執行。這三份檔案原樣保留自本機 EdSim51 資料夾，未重新驗證完整功能；來源作者未確認。

- `display_number.asm`：七段顯示器查表範例。
- `reading_digits.asm`：讀取數字範例。
- `reading_digits_interrupt.asm`：UART 中斷讀取與顯示範例。

## Runtime 與設定

`runtime/edsim51di.jar` 與 `runtime/lib/` 必須保留相對位置。啟動器固定從 runtime 目錄執行，模擬器產生的 `.ser` 設定檔由 Git 忽略。沒有帶入原機器的硬體設定；若作業需要特定接線，請依原 submission 報告設定。

[官方安裝說明](https://edsim51.com/installation-instructions/)提供新版下載。本 repo 保留課程使用的 2.1.38；升級可能改變行為。Java 與套件完整性已檢查，GUI 操作與 Windows launcher 未實機驗證。
