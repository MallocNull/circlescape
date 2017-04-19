@echo off
del lib.js > nul 2>&1
copy lib\*.js /a lib-raw.js /b > nul 2>&1
cmd /C "uglifyjs lib-raw.js -o bin\lib.js"
del lib-raw.js > nul 2>&1
