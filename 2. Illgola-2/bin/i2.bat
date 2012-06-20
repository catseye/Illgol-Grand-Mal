@echo off
perl i2pp.pl prj\%1.ill > tmp.ill
illgola2 tmp.ill %1.com
del tmp.ill
