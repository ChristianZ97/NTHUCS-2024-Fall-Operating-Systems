@echo off
cd /d "%~dp0runtime"
if defined JAVA_HOME (
  "%JAVA_HOME%\bin\java.exe" -jar edsim51di.jar
) else (
  java -jar edsim51di.jar
)
if errorlevel 1 pause
