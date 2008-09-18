#include "config.h"
#include <pthread.h>
#ifdef WITH_PTHREAD_NP
#include <pthread_np.h>
#endif
#include "decoderfactory.h"
#include "player.h"

void*
player_wrapper(void* data)
{
	Player* player = (Player*)data;

	/*
	 * XXX We do not acquire any locks here
	 */
	player->decoder->run();
	if (!player->decoder->isTerminating()) {
		/*
		 * Decoder was not forcefully terminated - signal player termination
		 */
		player->terminated();
		
	}
	return NULL;
}


Player::Player(std::string resource, Output* o, Visualizer* v)
{
	/*
	 * Note that it's not possible to chain constructers - so do what
	 * Player(...) does first...
	 */
	output = o; visualizer = v; input = NULL; decoder = NULL; info = NULL;
	playerPaused = false; havePlayerThread = false;
	pthread_mutex_init(&mtx_data, NULL);

	DecoderFactory::construct(resource, output, visualizer, &input, &decoder, &info);
}

Player::~Player()
{
	pthread_mutex_lock(&mtx_data);

	/* get rid of the player, if any - this will remove any lingering objects */
	stop_locked();

	pthread_mutex_destroy(&mtx_data);
}

void
Player::play_locked()
{
	stop_locked();
	pthread_create(&playerThread, NULL, player_wrapper, this);
	havePlayerThread = true;
}

void
Player::stop_locked()
{
	if (!havePlayerThread)
		return;

	/* Ask the decoder thread to terminate, and wait until it is gone */
	decoder->terminate();
	cont_locked();
	pthread_join(playerThread, NULL);

	delete input; delete info; delete decoder;
	input = NULL; info = NULL; decoder = NULL;
	havePlayerThread = false;
}

void
Player::pause_locked()
{
	if (playerPaused || !havePlayerThread)
		return;

#ifdef WITH_PTHREAD_NP
	pthread_suspend_np(playerThread);
#endif
	playerPaused = true;
}

void
Player::cont_locked()
{
	if (!playerPaused || !havePlayerThread)
		return;

#ifdef WITH_PTHREAD_NP
	pthread_resume_np(playerThread);
#endif
	playerPaused = false;
}

unsigned int
Player::getPlayingTime()
{
	pthread_mutex_lock(&mtx_data);

	unsigned int i;
	if (decoder == NULL)
		i = 0;
	else
		i = decoder->getPlayingTime();

	pthread_mutex_unlock(&mtx_data);
	return i;
}

unsigned int
Player::getTotalTime()
{

	pthread_mutex_lock(&mtx_data);

	unsigned int i;
	if (decoder == NULL)
		i = 0;
	else
		i = decoder->getTotalTime();

	pthread_mutex_unlock(&mtx_data);
	return i;
}

bool
Player::isPaused()
{
	pthread_mutex_lock(&mtx_data);
	bool b = playerPaused;
	pthread_mutex_unlock(&mtx_data);
	return b;
}

Info*
Player::getInfo()
{
	pthread_mutex_lock(&mtx_data);
	Info* i = info;
	pthread_mutex_unlock(&mtx_data);
	return i;
}

void
Player::play()
{
	pthread_mutex_lock(&mtx_data);
	play_locked();
	pthread_mutex_unlock(&mtx_data);
}

void
Player::stop()
{
	pthread_mutex_lock(&mtx_data);
	stop_locked();
	pthread_mutex_unlock(&mtx_data);
}

void
Player::pause()
{
	pthread_mutex_lock(&mtx_data);
	pause_locked();
	pthread_mutex_unlock(&mtx_data);
}

void
Player::cont()
{
	pthread_mutex_lock(&mtx_data);
	cont_locked();
	pthread_mutex_unlock(&mtx_data);
}
