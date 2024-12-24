# Cxx YouTube and Twitch fun stuff

This project is my way of having fun with YouTube and Twitch APIs. 

Points of possible interest if you want to develop something like that:
- Integration of oauth2 and some of API calls to YouTube is [here](src/api/youtube_api.cxx)
- Integration of oauth2 and some other API calls for Twitch is [here](src/api/twitch_api.cxx)
- Libraries required by project is [here](CMakeLists.txt#L13)
- Use your own secrets and do not commit them, [here](src/secrets/youtube-secret-template.h) and [here](src/api/twitch_api.cxx) you can find information you need
- List of used libraries [here](CMakeLists.txt#L13) and [here](CMakeLists.txt#L39)

In this project I used [SDL2](https://github.com/libsdl-org/SDL) as my windows/input/renderer to simplify things. For now it's just very simple engine that renders sprites on magenta background and mostly that's all.

Magenta background used so application could later by captured by OBS and color-key removed (but that don't work for me too great).
