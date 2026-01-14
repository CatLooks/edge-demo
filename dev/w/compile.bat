@echo off
py ../lib/map.py world.xml
py ../lib/route.py routes.xml
move /y *.dat ../../levels/