@echo off
pushd dev
py lib/map.py show.xml
py lib/map.py world.xml
py lib/route.py routes.xml
move /y *.dat ../levels/
popd