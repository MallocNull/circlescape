@echo off
del lib.js > nul 2>&1
copy raw\*.js /a lib-raw.js /b > nul 2>&1
cmd /C "uglifyjs lib-raw.js -o lib.js"
del lib-raw.js
