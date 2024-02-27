title "start-build-rustdesk"

echo "start kill rustdesk"
taskkill /im rustdesk.exe /f

echo "start cargo build..."
cargo build

copy target\debug\rustdesk.exe .\ /y

