@echo off
py lib/route.py routes.xml
move /y routes.dat ../levels/routes.dat