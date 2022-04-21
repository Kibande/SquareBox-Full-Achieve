#pragma once
#include "IPlayer.h"
#ifndef SQB_PLATFORM_ANDROID
typedef long int ssize_t;//must have for vlc
#else
//#include <GLES/gl.h>
#endif // !SQB_PLATFORM_ANDROID

#include<vlc/vlc.h>
#include <Asset-Manager/Asset-Manager.h>
#include <SquareBox-Logger/Logger.h>
#include<string>
#include<mutex>
#include<algorithm>
class VideoView {
public:

	unsigned char* pixelBuffer = nullptr;
	bool needUpdate = false;
	libvlc_instance_t *vlc = nullptr;
	libvlc_media_player_t *mediaPlayer = nullptr;
	unsigned int videoWidth, videoHeight, player_volume = 100;
	std::mutex vlcMutex;
	std::string chromFormat;
	SquareBox::AssetManager::GLTexture * p_texture = nullptr;
	bool finishedGettingDimensions = false;
	// constructor
	int init() {
		needUpdate = false;
		char const *vlc_argv[] =
		{
			"--no-video-title-show",
			"--ignore-config",
			//"--no-xlib", /* tell VLC to not use Xlib */
		};
		int vlc_argc = sizeof(vlc_argv) / sizeof(*vlc_argv);

		/* Load the VLC engine
		for this operation to be successful we need to have the vlc plug ins folder in our
		applications root directory

		*/

		vlc = libvlc_new(vlc_argc, vlc_argv);
		return 1;
	}
	void copyVLcInstance(libvlc_instance_t* _vlc) {
		vlc = _vlc;
	}

	libvlc_instance_t* getVlcInstance() {
		return vlc;
	}
	int setMediaSource(std::string videoPath, SquareBox::AssetManager::GLTexture * _texture, std::string _chromaFormat, bool _isRemoteVideo) {
		if (vlc != nullptr) {
			needUpdate = false;
			p_texture = _texture;
			chromFormat = _chromaFormat;
			/*
			setting the best dimensions for the video mean that accurate buffers will be created hence a clear picture
			*/
			videoWidth = 800;
			videoHeight = 300;

			accessMediaSource(videoPath, _isRemoteVideo);
			setupGL();
			return 1;
		}
		else {
			SBX_CORE_ERROR("VLC not initialised");
			return -1;
		}
	}

	~VideoView() {
		tearDownVLC();
	}

	static void *videoLockCallBack(void *object, void **planes) {
		VideoView* self = (VideoView *)object;
		self->vlcMutex.lock();
		planes[0] = (void *)self->pixelBuffer;
		return NULL;
	}

	static void videoUnlockCallback(void *object, void *picture, void * const *planes) {
		VideoView* self = (VideoView *)object;
		self->needUpdate = true;
		self->vlcMutex.unlock();
	}

	static void videoDisplayCallback(void *object, void *picture) {}

	void accessMediaSource(std::string videoPath, bool isRemoteVideo) {
		std::replace(videoPath.begin(), videoPath.end(), '/', '\\');
		libvlc_media_t *m = nullptr;

		/* Create a new item */
		if (isRemoteVideo) {
			m = libvlc_media_new_location(vlc, videoPath.c_str());
		}
		else {
			m = libvlc_media_new_path(vlc, videoPath.c_str());
		}
		/* Create a media player playing environment */
		if (mediaPlayer != nullptr) {
			libvlc_media_player_release(mediaPlayer);
			mediaPlayer = nullptr;
		}
		mediaPlayer = libvlc_media_player_new_from_media(m);

		/* No need to keep the media now */
		libvlc_media_release(m);

		pixelBuffer = new unsigned char[videoWidth*videoHeight * 3];
		libvlc_video_set_callbacks(mediaPlayer, videoLockCallBack, videoUnlockCallback, videoDisplayCallback, this);
		libvlc_video_set_format(mediaPlayer, chromFormat.c_str(), videoWidth, videoHeight, videoWidth * 3);
		libvlc_media_player_play(mediaPlayer);
	}

	void tearDownVLC() {
		if (vlc != nullptr) {
			libvlc_release(vlc);
			vlc = nullptr;
		}
	}

	void setupGL() {
		// Setup texture
		glGenTextures(1, &p_texture->id);
	}

	void pause() {
		if (mediaPlayer != nullptr)
			libvlc_media_player_pause(mediaPlayer);
		else {
			SBX_CORE_ERROR("No Media File was loaded");
		}
	}
	void play() {
		if (mediaPlayer != nullptr) {
			libvlc_media_player_play(mediaPlayer);
		}
		else {
			SBX_CORE_ERROR("No Media File was loaded");
		}
	}
	void stop() {
		if (mediaPlayer != nullptr) {
			libvlc_media_player_stop(mediaPlayer);
		}
		else {
			SBX_CORE_ERROR("No Media File was loaded");
		}
	}

	void setVolumeLevel(int vol) {
		player_volume = vol;
	}

	int getVolumeLevel() {
		return player_volume;
	}

	void updateTexture() {
		if (vlcMutex.try_lock()) {
			if (mediaPlayer != nullptr) {
				libvlc_audio_set_volume(mediaPlayer, player_volume);
			}
			if (!finishedGettingDimensions && mediaPlayer != nullptr) {
				videoWidth = libvlc_video_get_width(mediaPlayer);
				videoHeight = libvlc_video_get_height(mediaPlayer);
				if (videoWidth > 0 && videoHeight > 0) {
					delete pixelBuffer;
					pixelBuffer = new unsigned char[videoWidth*videoHeight * 3];
					libvlc_video_set_callbacks(mediaPlayer, videoLockCallBack, videoUnlockCallback, videoDisplayCallback, this);
					libvlc_video_set_format(mediaPlayer, chromFormat.c_str(), videoWidth, videoHeight, videoWidth * 3);
					libvlc_media_player_play(mediaPlayer);
					finishedGettingDimensions = true;
					needUpdate = false;
				}
			}
			if (needUpdate) {
				//creating our opengl texture
				glBindTexture(GL_TEXTURE_2D, p_texture->id);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				//set up some texture parameters
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//liner sampling
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//magnification
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//yes tiling in y
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//yes tiling  in x
				//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				//send the data to Opengl

#ifndef SQB_PLATFORM_ANDROID
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, videoWidth, videoHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixelBuffer);
#else
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, videoWidth, videoHeight, 0, 0, GL_UNSIGNED_BYTE, pixelBuffer);

#endif // !SQB_PLATFORM_ANDROID

				//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, videoWidth, videoHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelBuffer);
				glGenerateMipmap(GL_TEXTURE_2D);
				p_texture->width = videoWidth;
				p_texture->height = videoHeight;
				//unbind the texture

				glBindTexture(GL_TEXTURE_2D, 0);

				needUpdate = false;
			}
			vlcMutex.unlock();
		}
	}
};

namespace SquareBox {
	namespace MediaPlayer {
		class VLCplayer :public IPlayer
		{
		public:
			VLCplayer();
			~VLCplayer();

			// Inherited via IPlayer
			virtual int init() override;
			virtual void play() override;
			virtual int setMediaSource(std::string _filePath, SquareBox::AssetManager::GLTexture * _texture, std::string _chromaFormat, bool _isRemoteVideo = false) override;
			virtual void decode() override;
			virtual void pause() override;
			virtual void stop() override;
			virtual void setVolumeLevel(int _level) override;
			virtual int getVolumeLevel() override;
			virtual void dispose() override;
			void copyVLcInstance(libvlc_instance_t* _vlc) {
				p_player.copyVLcInstance(_vlc);
			}

			libvlc_instance_t* getVlcInstance() {
				return p_player.getVlcInstance();
			}
		private:
			VideoView p_player;
		};
	};
};