#include "config.h"
#include <pthread.h>
#include <signal.h>
#include "decoderfactory.h"
#include "exceptions.h"
#include "player.h"

using namespace std;

void*
player_wrapper(void* data)
{
	Player* player = (Player*)data;

	/*
	 * Explicitely block most signals from reaching us. This causes
	 * possible stuttering if the application uses things like SIGALRM,
	 * SIGHUP etc while we are being suspended.
	 *
	 * One may argue whether this is the correct place to do this; one
	 * way of reasoning is that only a Player thread suffers from these
	 * problems, so it wouldn't be fair to force an application to block
	 * signals only because this thread can't deal with them...
	 */
	sigset_t smask;
	sigemptyset(&smask);
	sigaddset(&smask, SIGALRM); sigaddset(&smask, SIGHUP);
	pthread_sigmask(SIG_BLOCK, &smask, NULL);

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


Player::Player(string resource, Output* o, Visualizer* v)
{
	/*
	 * Note that it's not possible to chain constructers - so do what
	 * Player(...) does first...
	 */
	output = o; visualizer = v; input = NULL; decoder = NULL; info = NULL;
	playerPaused = false; havePlayerThread = false;
	pthread_mutex_init(&mtx_data, NULL);
	pthread_cond_init(&cv_suspend, NULL);

	DecoderFactory::construct(resource, this, output, visualizer, &input, &decoder, &info);
	if (decoder == NULL)
		throw PlayerException("no decoder available");
}

Player::~Player()
{
	pthread_mutex_lock(&mtx_data);

	/* get rid of the player, if any - this will remove any lingering objects */
	stop_locked();
	pthread_mutex_unlock(&mtx_data);

	pthread_mutex_destroy(&mtx_data);
	pthread_cond_destroy(&cv_suspend);
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

	/*
	 * Note that while waiting for the thread to die, we have to let go of
	 * the mutex. This is because it may be waiting for a condition variable
	 * (for example, if it was paused) and we don't want it to get stuck
	 * waiting for a mutex that never gets unlocked...
	 */
	pthread_mutex_unlock(&mtx_data);
	pthread_join(playerThread, NULL);
	pthread_mutex_lock(&mtx_data);

	delete input; delete info; delete decoder;
	input = NULL; info = NULL; decoder = NULL;
	havePlayerThread = false;
}

void
Player::pause_locked()
{
	if (playerPaused || !havePlayerThread)
		return;

	// Decoder threads will automatically suspend themselves if this is set
	playerPaused = true;
}

void
Player::cont_locked()
{
	if (!playerPaused || !havePlayerThread)
		return;

	playerPaused = false;
	pthread_cond_signal(&cv_suspend);
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

bool
Player::isStopped()
{
	pthread_mutex_lock(&mtx_data);
	bool b = !havePlayerThread;
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

void
Player::handleUnpause()
{
	/*
	 * Our pausing/resuming works by having the decoder thread
	 * check whether the player should be paused. If this is the
	 * cause, it waits until the 'suspend' condition variable
	 * is signalled.
	 *
	 * Thus, in order to pause, all we need to do is set the
	 * player paused variable to true, which will cause this code
	 * to suspend the decoder.
	 *
	 * As to unpause, we first set the paused variable to false,
	 * and signal the 'suspend' condition variable. This causes
	 * the player to resume itself, yet it will immediately
	 * continue playing since the paused variable is false.
	 */
	pthread_mutex_lock(&mtx_data);
	if (playerPaused) {
		/* We are paused - wait until we can play again */
		pthread_cond_wait(&cv_suspend, &mtx_data);
	}
	pthread_mutex_unlock(&mtx_data);
}
