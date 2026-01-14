#pragma once

/// User interface namespace.
namespace UI {
	/// Element alignment mode.
	enum Align: char {
		Left   = -1,
		Center =  0,
		Right  =  1,
	};
};

// import UI units
#include "ui/sprite.hpp"
#include "ui/class.hpp"
#include "ui/animator.hpp"
#include "ui/screen.hpp"
#include "ui/panel.hpp"
#include "ui/text.hpp"
#include "ui/popup.hpp"
#include "ui/button.hpp"
#include "ui/menu.hpp"
#include "ui/hangtime.hpp"
#include "ui/pause.hpp"
#include "ui/level.hpp"
#include "ui/blind.hpp"
#include "ui/rank.hpp"
#include "ui/complete.hpp"
#include "ui/icon.hpp"
#include "ui/world.hpp"
#include "ui/stars.hpp"
#include "debug.hpp"

/// User interface namespace.
namespace UI {
	/// UI system namespace.
	class System : public Screen {
		public:
		/// Game state enumeration.
		enum State {
			Load    , // Game startup.
			WorldMap, // World map navigation.
			GameLoad, // Level loading screen.
			GameMap , // Level gameplay.
			GameExit, // Level complete menu.
		};
		/// Game state.
		State state = Load;

		protected:
		/// Checks whether world map needs to be drawn.
		bool drawWorldMap() const { return state == Load || state == WorldMap; };
		/// Checks whether game map needs to be drawn.
		bool drawGameMap() const { return state == GameLoad || state == GameMap || state == GameExit; };

		/// Animator list.
		std::list<std::unique_ptr<Animator>> anims;
		/// Background star system.
		StarSystem stars;
		/// Pause menu.
		Pause* pause = NULL;
		/// Level completion menu dummy.
		Element* dummy = NULL;
		/// Level completion menu.
		Complete* comp = NULL;
		/// Edge time label.
		Hangtime* label_edge = NULL;
		/// Level info label.
		LevelInfo* label_info = NULL;
		/// World map manager.
		World* world_mgr = NULL;
		/// Transition animation blind.
		Blind* blind = NULL;
		/// Level loading screen.
		Wall* wall = NULL;

		/// Cube input recorder.
		Level::Recorder recorder = Level::Recorder(stdout, NULL);

		/// Whether to do next level animation.
		bool nextAnim = false;
		/// Whether to immediately enter next level.
		bool nextPlay = false;
		/// Whether a cube animation is in progress.
		bool cubeAnim = false;
		/// Whether the pause menu is active.
		bool isPaused = false;
		/// Whether the pause menu can be toggled.
		bool canPause = true;
		/// Whether all required game assets have been loaded.
		bool ready = true;
		/// Whether to overwrite world map keymaps.
		bool debug = false;

		/// Exit timer.
		float exittime = 0.0f;
		/// Whether the exit is in progress.
		bool exitprog = false;

		public:
		/// Notification manager.
		NotifManager* notifs;
		/// Level file collection.
		Data::World world;
		/// Worldmap level.
		std::unique_ptr<Level::Level> worldmap = NULL;
		/// Game level.
		std::unique_ptr<Level::Level> gamemap = NULL;

		/// System constructor.
		/// @param routes Worldmap route data.
		System (const Bytes::Routes& routes): world(routes) {
			// create GUI elements
			add(label_edge = new Hangtime(NULL));
			add(label_info = new LevelInfo(NULL));
			add(world_mgr = new World(world));
			add(pause = new Pause(NULL, 0));
			add(dummy = new Element);

			// create overlay elements
			add(blind = new Blind());
			add(wall = new Wall());
			add(notifs = new NotifManager(&anims));
			Debug::genProfile(*this);

			// generate level select menu animations
			world_mgr->emitIconAnims(anims);

			// disable cursor
			app.setMouseCursorVisible(false);

			// load progress
			loadProgress();

			// request handler callback
			updateCall([&](Element&, float delta) {
				// trigger flags
				bool triggerWorldMap = false;

				// check for pause menu toggle request
				if (pause->toggle) {
					pause->toggle = false;
					togglePause();
				};

				// check for pause menu world map exit request
				if (pause->world) {
					pause->world = false;
					pause->ignore = true;
					triggerWorldMap = true;
				};

				// check for level exit request
				if (state == GameMap && gamemap && gamemap->exitReq) {
					gamemap->exitReq = false;
					levelComplete();
				};

				// world map menu requests
				if (world_mgr) {
					// check for level info display request
					if (world_mgr->infoReq) {
						world_mgr->infoReq = false;
						updateInfo();
					};

					// check for level start request
					if (world_mgr->playReq) {
						world_mgr->playReq = false;
						world_mgr->ignore = true;
						startLevel();
					};
				};

				// level complete screen requests
				if (comp) {
					bool pressed = false;

					// exit to next level
					if (comp->req == Complete::Next) {
						pressed = true;

						// play new level animation
						if (nextAnim) {
							triggerWorldMap = true;
							nextPlay = true;
						}

						// start next level
						else {
							if (++world_mgr->selected >= world_mgr->icons.size()) {
								world_mgr->selected--;
								triggerWorldMap = true;
								nextPlay = false;
							} else {
								updateInfo();
								world_mgr->moveCube();
								world_mgr->infoReq = false;
								label_info->active = false;
								startLevel();
							};
						};
					}

					// retry level
					else if (comp->req == Complete::Retry) {
						pressed = true;
						startLevel();
					}

					// exit to world map
					else if (comp->req == Complete::Exit) {
						pressed = true;
						triggerWorldMap = true;
					};

					// hide menu if a button is pressed
					if (pressed) {
						comp->ignore = true;

						// create exit animation
						AnimDim* anim = new AnimDim;
						anim->start(&comp->m_base->box.pos.y, -1ps, 0.3f);
						anim->finish([=](AnimDim&) {
							AnimDim* anim = new AnimDim;
							anim->start(&comp->box.pos.x, -1ws, 0.15f);
							animate(anim);
						});
						animate(anim);
					};

					// reset request variable
					comp->req = Complete::None;
				};

				// execute world map transition
				if (triggerWorldMap) {
					// fade out music
					Music::fade();

					// create blackout transition
					transition(0.5f, [&] {
						// change state to world map
						state = WorldMap;
						if (worldmap) worldmap->reapply();
						app.setMouseCursorVisible(true);

						// activate level select menu
						if (world_mgr) {
							world_mgr->active = true;
							world_mgr->ignore = true;
						};

						// hide pause menu
						if (isPaused) {
							pause->box.pos.x = -1ws;
							pause->m_base->box.pos.y = -1ps;
							pause->active = false;
						};

						// destroy level complete menu
						if (comp) {
							swap(comp, dummy = new Element);
							comp = NULL;
						};

						// request world map element updates
						if (world_mgr) world_mgr->recolorIcons(world);
						updateStats(); updateInfo();
					}, [&] {
						// enable world menu
						if (!nextAnim) {
							world_mgr->ignore = false;
							worldmap->contactcall();
							return;
						};

						// start cube animation
						cubeNext();
					});
				};
			});

			// level update callback
			updateCall([&](Element&, float delta) {
				// update star system
				stars.update(delta);

				// update loading screen
				if (state == GameLoad) {
					// generate gamemap transition
					if (Assets::nameSfxDone() && wall->ignore) {
						wall->ignore = false;

						// start fade out animation
						AnimFloat* anim = new AnimFloat;
						anim->start(&wall->blind->alpha, 1.0f, 0.5f);
						anim->finish([&](AnimFloat&) {
							// start game
							state = GameMap;
							isPaused = false;
							canPause = true;

							// tick game map
							gamemap->update(0.0f);
							gamemap->follow(glm::vec3(0.0f));

							// hide world menu elements
							wall->active = false;
							label_info->active = false;
							world_mgr->active = false;
							world_mgr->ignore = true;

							// fade out blind
							AnimFloat* anim = new AnimFloat;
							anim->start(&blind->alpha, 0.0f, 0.5f);
							anim->finish([&](AnimFloat&) {
								blind->active = false;
							});
							animate(anim);
						});
						animate(anim);
					};
				};

				// update game map
				if (gamemap && drawGameMap()) {
					// update game state
					if (state != GameLoad && !isPaused) gamemap->update(delta);

					// update camera position
					if (state == GameMap)
						gamemap->follow(isPaused ? pause->cameraOffset(gamemap->camera) : glm::vec3(0.0f));
					else if (state == GameExit)
						gamemap->follow(comp->cameraOffset(gamemap->camera));
				};

				// update world map
				if (worldmap && drawWorldMap()) {
					// update world state
					worldmap->update(delta, debug);

					// check for world map exit
					if (worldmap->getPlayer().act == Level::Cube::Exit) {
						if (world_mgr->selected != 0) {
							world_mgr->ignore = false;
							world_mgr->selected = 0;
							worldmap->contactcall();
							updateInfo();
						};
					};

					// exit cube animation if needed
					if (cubeAnim && !worldmap->getPlayer().keys.active()) {
						// activate world menu
						cubeAnim = false;
						if (!(world_mgr->ignore = nextPlay))
							app.setMouseCursorVisible(true);

						// show new level info
						if (++world_mgr->selected >= world_mgr->icons.size()) {
							world_mgr->selected--;
							worldmap->contactcall();
						} else {
							updateInfo();
							if (nextPlay) animate(new AnimVoid(0.5f, [&](AnimVoid&) {
								// start new level
								nextPlay = false;
								startLevel();
							}));
						};
					};

					// update loading state
					if (state == Load) {
						if (ready) {
							// wait until cube collides with ground
							if (worldmap->getPlayer().act == Level::Cube::Idle) {
								// animate world level select menu
								world_mgr->emitEntry(anims);
								world_mgr->recolorIcons(world);

								// transition into main menu state
								state = WorldMap;
								updateInfo();
								app.setMouseCursorVisible(true);
							};
						}

						// reset cube position if not loaded yet
						else worldmap->getPlayer().tile = worldmap->root.spawn;
					};

					// update camera position
					if (state == WorldMap)
						worldmap->follow(world_mgr->cameraOffset(worldmap->camera));
					else
						worldmap->follow(glm::vec3(0.0f));
				};

				// update recorder
				recorder.update(delta);
			});

			// animation update callback
			updateCall([&](Element&, float delta) {
				// tick animations
				auto it = anims.begin();
				while (it != anims.end()) {
					// tick current animation
					(*it)->update(delta);

					// advance iterator
					if ((*it)->active())
						it++;
					else
						it = anims.erase(it);
				};
			});

			// exit timer update callback
			updateCall([&](Element&, float delta) {
				exittime = glm::max(exittime - delta, 0.0f);
			});

			// exit callback
			eventCall(KeyDown, [&](Element&, const EventData& data) -> bool {
				// ignore if not in world menu
				if (!(state == Load || state == WorldMap))
					return false;

				// ignore if not pressing escape
				if (data.key != sf::Keyboard::Escape)
					return false;

				// ignore if exit is in progress
				if (exitprog)
					return true;

				// check if the timer has run out
				if (exittime == 0.0f) {
					// create sound effect
					Assets::sfx(Assets::Cancel);

					// push exit notification
					notifs->push(Notif::Once([](Text& text) {
						text.setText("\2Press <\6Escape\7> again to exit.");
					}));

					// reset exit timer
					exittime = 2.0f;
					return true;
				};

				// pop all notifications
				notifs->pop();

				// prepare exit sound
				Assets::sfx(Assets::Start);
				Music::fade();

				// disable world menu
				world_mgr->ignore = true;
				exitprog = true;

				// create exit blackout
				transition(1.2f, [&]() { app.close(); });
				return true;
			});

			// pause toggle callback
			eventCall(KeyDown, [&](Element&, const EventData& data) -> bool {
				// ignore if not in game
				if (state != GameMap)
					return false;

				// toggle pause if a key is pressed
				if (comp == NULL) {
					if (data.key == sf::Keyboard::Escape || data.key == sf::Keyboard::Enter) {
						bool req = togglePause();
						if (req && !isPaused) Assets::sfx(Assets::Cancel);
					};
				};

				// no events captures
				return false;
			});

			// cube walk animation playback
			eventCall(KeyDown, [&](Element&, const EventData& data) -> bool {
				// ignore if not in idle worldmap
				if (state != WorldMap || cubeAnim)
					return false;

				// start cube animation
				if (data.key == sf::Keyboard::Space) {
					// check if walk is possible
					if (world_mgr->selected + 1 < world_mgr->icons.size() && world_mgr->icons[world_mgr->selected + 1]->locked)
						return false;

					// request animation
					nextPlay = false;
					cubeNext();
					return true;
				};

				// event not handled
				return false;
			});

			// debug mode enable key combo
			updateCall([&](Element&, float) {
				static bool added = false;
				if (!added
					&& sf::Keyboard::isKeyPressed(sf::Keyboard::Tab)
					&& sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)
					&& sf::Keyboard::isKeyPressed(sf::Keyboard::F3))
				{
					addDebugCallbacks();
					added = true;
				};
			});
		};

		/// Enables debug mode.
		void addDebugCallbacks() {
			// recorder control callback
			eventCall(KeyDown, [&](Element&, const EventData& data) -> bool {
				// enable debug mode / increase position index
				if (data.key == sf::Keyboard::PageUp) {
					// debug mode toggle
					if (state == WorldMap) {
						debug = true;
						notifs->push(Notif::Once([](Text& text) {
							text.setFmt("\2Debug mode enabled.");
						}));
						return true;
					};

					// teleport player cube
					if (gamemap && state == GameMap) {
						size_t poi = gamemap->warp(1);
						if (poi != -1ULL) notifs->push(Notif::Once([=](Text& text) {
							text.setFmt("\2Warped to darkcube <\6%d\7>.", poi);
						}));
						recorder.start();
						return true;
					};
				};

				// disable debug mode / decrease position index
				if (data.key == sf::Keyboard::PageDown) {
					// debug mode toggle
					if (state == WorldMap) {
						debug = false;
						notifs->push(Notif::Once([](Text& text) {
							text.setFmt("\2Debug mode disabled.");
						}));
						return true;
					};

					// teleport player cube
					if (gamemap && state == GameMap) {
						size_t poi = gamemap->warp(-1);
						if (poi != -1ULL) notifs->push(Notif::Once([=](Text& text) {
							text.setFmt("\2Warped to darkcube <\6%d\7>.", poi);
						}));
						recorder.start();
						return true;
					};
				};

				// bind recorder
				if (data.key == sf::Keyboard::Insert) {
					if (worldmap && state == WorldMap) {
						// bind to world map
						recorder.cube = &worldmap->getPlayer();
						recorder.reset();
						notifs->push(Notif::Once([](Text& text) {
							text.setFmt("\2Recorder bound to \6world map\7.");
						}));
						return true;
					};
					if (gamemap && state == GameMap) {
						// bind to game map
						recorder.cube = &gamemap->getPlayer();
						recorder.reset();
						notifs->push(Notif::Once([](Text& text) {
							text.setFmt("\2Recorder bound to \6game map\7.");
						}));
						return true;
					};
				};

				// unbind recorder
				if (data.key == sf::Keyboard::Delete) {
					recorder.cube = NULL;
					notifs->push(Notif::Once([](Text& text) {
						text.setFmt("\2Recorder unbound.");
					}));
					return true;
				};

				// reset recorder time
				if (data.key == sf::Keyboard::Home) {
					recorder.reset();
					notifs->push(Notif::Once([](Text& text) {
						text.setFmt("\2Recorder reset.");
					}));
					return true;
				};

				// create sync message
				if (data.key == sf::Keyboard::Pause) {
					recorder.sync();
					notifs->push(Notif::Once([](Text& text) {
						text.setFmt("\2" "Sync request created.");
					}));
					return true;
				};

				// finish recording
				if (data.key == sf::Keyboard::End) {
					recorder.end();
					notifs->push(Notif::Once([](Text& text) {
						text.setFmt("\2Recording saved.");
					}));
					return true;
				};

				// no events captured
				return false;
			});
		};

		/// Starts to play a music track.
		/// @param theme Music track.
		void setMusic(Music::Music theme) {
			if (!Music::start(theme)) {
				notifs->push(Notif::Once([](Text& text) {
					text.setFmt("\2Failed to load music.");
				}));
			};
		};

		/// Adds music on first contact in the level.
		/// @param theme Music track.
		/// @param level Level pointer.
		void addMusic(Music::Music theme, Level::Level* level) {
			level->contactcall = [&, theme]() {
				setMusic(theme);
			};
		};

		/// Checks whether the timers need updates.
		bool stoptimers() const { return isPaused; };

		/// Loads a map.
		/// @param path Map file path.
		const Bytes::Level& loadMap(const std::string& path) {
			const Bytes::Level& level = world.load(path);
			if (world.error) {
				// display error
				notifs->push(Notif::Once([=](Text& text) {
					text.setFmt(
						"\2Couldn't open \x19\6<%s>\7: \x17\6%s",
						path.c_str(), strerror(world.error)
					);
				}));
			};
			return level;
		};

		/// Stars cube walk animation.
		void cubeNext() {
			// prepare walk key map
			label_info->emitHide(anims);
			cubeAnim = true;
			nextAnim = false;
			worldmap->getPlayer().keys
				= Level::KeyMap(&world.routedata.routes[world_mgr->selected].path);
			world_mgr->ignore = true;

			// reset camera triggers
			worldmap->state.last = -1ULL;
		};

		/// Preloads level signatures.
		void loadSigns() {
			auto err = world.preload();
			for (const auto& info : err) {
				// create notifications
				notifs->push(Notif::Once([=](Text& text) {
					text.setFmt(
						"\2Couldn't open \x19\6<%s>\7: \x17\6%s",
						info.first.c_str(), strerror(info.second)
					);
				}));
			};
		};

		/// Updates level select menu stats.
		void updateStats() { world_mgr->updateStats(world); };

		/// Updates level info display.
		void updateInfo() {
			// ignore if display is not instantiate
			if (label_info == NULL) return;

			// get level signature
			const std::string& file = world.routedata.routes[world_mgr->selected].file;
			Bytes::Sign sign = world.findFile(file);

			// validate level signature
			if (sign.ID == 0) {
				notifs->push(Notif::Once([=](Text& text) {
					text.setFmt("\2\x19" "Failed to load level \6<%s>", file.c_str());
				}));
				return;
			};

			// get level data
			Data::Record rec = world.save.get(sign.ID);
			Data::LevelData data = world.merge(sign, rec, (uint16_t)world_mgr->selected + 1);

			// update level info display
			label_info->pos = world.routedata.routes[world_mgr->selected].pos;
			label_info->data = data;
			label_info->display();
			label_info->update(0.0f);
			label_info->emitShow(anims);
		};

		/// Loads world level.
		/// @param data Level data.
		void loadWorld(const Bytes::Level& data) {
			// create world level object
			worldmap = std::unique_ptr<Level::Level>(new Level::Level(data, true));
			worldmap->effect(world.routedata, world.levelNos());
			worldmap->model();
			worldmap->initCheckpoint();
			addMusic(worldmap->root.mus, worldmap.get());

			// bind world map to world menu
			world_mgr->level = worldmap.get();

			// generate level info label
			LevelInfo* old = label_info;
			swap(old, label_info = new LevelInfo(&worldmap->camera));
		};

		/// Loads game level.
		/// @param data Level data.
		/// @param id Level ID.
		void loadLevel(const Bytes::Level& data, uint16_t id) {
			// delete debug data for previous level
			if (gamemap) remove(Debug::debug);

			// create game level object
			gamemap = std::unique_ptr<Level::Level>(new Level::Level(data, false));
			gamemap->model();
			gamemap->initCheckpoint();
			addMusic(gamemap->root.mus, gamemap.get());

			// create new game UI elements
			UI::Pause* pause_prev = pause;
			swap(pause_prev, pause = new Pause(gamemap.get(), id));
			UI::Hangtime* edge_prev = label_edge;
			swap(edge_prev, label_edge = new Hangtime(gamemap.get()));

			// setup pause menu
			pause->ignore = true;
			isPaused = false;
			canPause = true;

			// create debug textbox
			Debug::genDebug(*this, gamemap.get());
		};

		/// Creates a blind transition.
		/// @param time Blackout time.
		/// @param action What to do in the middle.
		/// @param after What to do at the end.
		void transition(float time, std::function<void()> action, std::function<void()> after = []{}) {
			blind->active = true;

			// create blackout
			AnimFloat* anim = new AnimFloat;
			anim->finish([=](AnimFloat&) {
				// invoke callback
				action();

				// fade away blind
				AnimFloat* anim = new AnimFloat;
				anim->start(&blind->alpha, 0.0f, time);
				anim->finish([&](AnimFloat&) {
					blind->active = false;

					// invoke callback
					after();
				});
				animate(anim);
			});
			anim->start(&blind->alpha, 1.0f, time);
			animate(anim);
		};

		/// Prepares a game level.
		void startLevel() {
			// load level data
			const std::string& file = world.routedata.routes[world_mgr->selected].file;
			Bytes::Level data = world.load(file);
			if (world.bad) {
				notifs->push(Notif::Once([=](Text& text) {
					text.setFmt("\2\x19" "Level \6<%s>\7 is \x17\6corrupted", file.c_str());
				}));

				// restore world menu state
				world_mgr->ignore = false;
				return;
			};

			// fade out menu music
			Music::fade();

			// create blackout
			app.setMouseCursorVisible(false);
			blind->active = true;
			AnimFloat* anim = new AnimFloat;
			anim->finish([&, file](AnimFloat&) {
				state = GameLoad;

				// destroy level complete menu
				if (comp) {
					swap(comp, dummy = new Element);
					comp = NULL;
				};

				// get level info
				Bytes::Sign sign = world.findFile(file);
				Data::Record record = world.save.get(sign.ID);

				// create level wall
				Wall* old = wall;
				swap(old, wall = new Wall(sign, record, (uint16_t)world_mgr->selected + 1));

				// fade in level wall
				AnimFloat* anim = new AnimFloat;
				anim->start(&wall->blind->alpha, 0.0f, 0.5f);
				anim->finish([=](AnimFloat&) {
					// play level name sound
					Assets::nameSfx(sign.ID);

					// load level
					loadLevel(world.load(file), (uint16_t)world_mgr->selected + 1);
					// unload level data
					world.unload(file);

					// create minimal waiting time
					animate(new AnimVoid(0.5f, [&](AnimVoid&) {
						wall->ignore = true;
					}));
				});
				animate(anim);
			});
			anim->start(&blind->alpha, 1.0f, 0.5f);
			animate(anim);
		};

		/// Toggles game pause.
		/// @return Whether the menu was toggled.
		bool togglePause() {
			// ignore if game is not active
			if (state != GameMap) return false;
			// ignore if cannot pause
			if (!canPause) return true;

			// get pause state
			if (isPaused) {
				// resume long sounds
				Assets::resumeSfx();

				// disable pause menu
				app.setMouseCursorVisible(false);
				pause->ignore = true;

				// start pause panel animation
				AnimDim* anim = new AnimDim;
				anim->start(&pause->m_base->box.pos.y, -1ps, 0.1f);
				anim->finish([&](AnimDim&) {
					canPause = true;

					// start pause menu animation
					AnimDim* anim = new AnimDim;
					anim->start(&pause->box.pos.x, -1ws, 0.1f);
					anim->finish([&](AnimDim&) {
						pause->active = false;
					});
					animate(anim);
				});
				animate(anim);
			} else {
				// show pause menu
				app.setMouseCursorVisible(true);
				pause->active = true;
				pause->m_menu->selected = 0;
				pause->ignore = false;

				// pause long sounds
				Assets::pauseSfx();
				Assets::sfx(Assets::Select);

				// start pause panel animation
				AnimDim* anim = new AnimDim;
				anim->start(&pause->box.pos.x, 0.1ps, 0.1f);
				anim->finish([&](AnimDim&) {
					canPause = true;

					// start pause menu animation
					AnimDim* anim = new AnimDim;
					anim->start(&pause->m_base->box.pos.y, 0ps, 0.1f);
					animate(anim);
				});
				animate(anim);
			};

			// update state
			isPaused = !isPaused;
			canPause = false;
			return true;
		};

		/// Completes a level.
		void levelComplete() {
			// ignore if no level is loaded
			if (state != GameMap || gamemap == NULL) return;
			canPause = false;

			// stop level music
			Music::fade();

			// store level record
			Data::RecordType type = world.store(gamemap->rank());
			if (type != Data::Old) {
				notifs->push(Notif::Once([&](Text& text) {
					text.setFmt("\2\x19\6New record!");
				}));
			};
			nextAnim = type == Data::Add;

			// save records
			saveProgress();

			// generate level completion menu
			swap(dummy, comp = new UI::Complete(gamemap->rank().setNo(pause->no), gamemap->root.name.c_str()));
			comp->m_img->box.size.y = 0px;
			comp->ignore = true;

			// get level size
			vec3x size = vec3x(
				gamemap->terrain.root.size.x,
				gamemap->terrain.root.size.y,
				gamemap->terrain.root.size.z
			);
			glm::vec3 glsize = size.gl();

			// get level screen bounding box
			//sf::IntRect rect = gamemap->camera.vertBounds(vol3x(size));

			// calculate level screen dimensions
			float width = glsize.x > glsize.z
				? glsize.z + (glsize.x - glsize.z) * 0.5f
				: glsize.x + (glsize.z - glsize.x) * 0.5f;

			// get block size
			sf::IntRect block = gamemap->camera.vertBounds(vol3x(gamemap->root.exit, vec3x(1)));

			// get scaling factors
			//float xscale = rect.width / (Win::size.x * 0.6f);
			//float yscale = rect.height / (Win::size.y * 0.9f);
			//float scale = xscale > yscale ? xscale : yscale;
			float scale = block.width * width / (Win::size.x * 0.6f);

			// update scaling factor
			scale *= tanf(glm::radians(22.0f));
			//scale = 5.0f;

			// create camera panning
			AnimFloat* anim = new AnimFloat;
			anim->setEasing(Easings::quadIn);
			anim->start(&gamemap->followPos.y, gamemap->followPos.y + 5.0f, 1.6f);
			animate(anim);

			// wait for FOV normalization
			animate(new AnimVoid(1.0f, [&, glsize, scale, size](AnimVoid&) {
				// wait for camera pan
				animate(new AnimVoid(0.8f, [=](AnimVoid&) {
					comp->active = true;

					// start level complete menu animation
					AnimDim* anim = new AnimDim;
					anim->setEasing(Easings::sine);
					anim->start(&comp->box.pos.x, 0.1ps, 0.6f);
					anim->finish([&, size](AnimDim&) {
						app.setMouseCursorVisible(true);

						// start menu animation
						AnimDim* anim = new AnimDim;
						anim->setEasing(Easings::sineOut);
						anim->start(&comp->m_base->box.pos.y, 0ps, 0.7f);
						anim->finish([&, size](AnimDim&) {
							// reveal rank
							AnimDim* anim = new AnimDim;
							anim->setEasing(Easings::sineOut);
							anim->start(&comp->m_img->box.size.y, comp->m_img->box.size.x, 0.2f);
							animate(anim);

							// activate menu
							comp->ignore = false;
							Assets::sfx(Assets::Select);
						});
						animate(anim);
					});

					// update state
					state = GameExit;
					canPause = false;
					animate(anim);

					// start level completion music
					Music::start(Music::Debrief);

					// update camera offset for full level capture
					gamemap->camera.targetOff(gamemap->camera.off * scale, 0.4f);

					// lock on level center
					gamemap->followLock = true;
					gamemap->followPos = glm::vec3(glsize.x, glsize.y * 0.5f, glsize.z) * 0.5f;
				}));
			}));
		};

		/// Writes save data to a file.
		void saveProgress() {
			int status = 0;
			FileWriter file = FileWriter::open("save.dat", &status);
			if (status) {
				notifs->push(Notif::Once([&](Text& text) {
					text.setFmt("\2\x17\6Failed to write to save file:\n%s", strerror(status));
				}));
			} else world.save.write(file);
		};

		/// Loads save data from a file.
		void loadProgress() {
			int status = 0;
			FileReader file = FileReader::open("save.dat", &status);
			if (!(status == 0 || status == ENOENT)) {
				notifs->push(Notif::Once([&](Text& text) {
					text.setFmt("\2\x17\6Failed to read save file: %s", strerror(status));
				}));
			} else if (status == 0)
				world.save.read(file);
		};

		protected:
		/// Registers an animation.
		/// @param anim Animation object.
		void animate(Animator* anim) {
			anims.push_back(std::unique_ptr<Animator>(anim));
		};

		/// Draws the GUI.
		/// @param self Bounding box.
		void drawSelf(sf::IntRect self) const override {
			// reset render stats
			UI::sprite_count = 0;
			Debug::vert_normal = 0;
			Debug::vert_shadow = 0;

			// clear screen
			Win::clear();

			// draw stars
			glDisable(GL_DEPTH_TEST);
			stars.draw(self);
			glEnable(GL_DEPTH_TEST);

			// draw game map
			if (drawGameMap() && gamemap) {
				Debug::vert_normal += gamemap->draw();   Win::buffer();
				Debug::vert_shadow += gamemap->shadow(); Win::shadow();
			};

			// draw world map
			if (drawWorldMap() && worldmap) {
				Debug::vert_normal += worldmap->draw();   Win::buffer();
				Debug::vert_shadow += worldmap->shadow(); Win::shadow();
			};

			// start drawing GUI
			Win::gui();
		};
	};
};