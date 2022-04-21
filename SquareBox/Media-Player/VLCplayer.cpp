#include "VLCplayer.h"

int SquareBox::MediaPlayer::VLCplayer::init()
{
	return p_player.init();
}

int SquareBox::MediaPlayer::VLCplayer::setMediaSource(std::string _filePath, SquareBox::AssetManager::GLTexture * _texture, std::string _chromaFormat, bool _isRemoteVideo)
{
	return p_player.setMediaSource(_filePath, _texture, _chromaFormat, _isRemoteVideo);
}

void SquareBox::MediaPlayer::VLCplayer::setVolumeLevel(int _level)
{
	p_player.setVolumeLevel(_level);
}

int SquareBox::MediaPlayer::VLCplayer::getVolumeLevel()
{
	return p_player.getVolumeLevel();
}

void SquareBox::MediaPlayer::VLCplayer::play()
{
	p_player.play();
}

void SquareBox::MediaPlayer::VLCplayer::pause()
{
	p_player.pause();
}

void SquareBox::MediaPlayer::VLCplayer::stop()
{
	p_player.stop();
}

void SquareBox::MediaPlayer::VLCplayer::decode()
{
	p_player.updateTexture();
}

void SquareBox::MediaPlayer::VLCplayer::dispose()
{
	p_player.tearDownVLC();
}

SquareBox::MediaPlayer::VLCplayer::VLCplayer()
{
}

SquareBox::MediaPlayer::VLCplayer::~VLCplayer()
{
}