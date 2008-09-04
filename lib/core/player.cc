#include <pthread_np.h>
#include "decoderfactory.h"
#include "player.h"

void*
player_wrapper(void* data)
{
	Player* player = (Player*)data;

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

	DecoderFactory::construct(resource, output, visualizer, &input, &decoder, &info);
}

Player::~Player()
{
	/* get rid of the player, if any */
	stop();

	/* remove lingering objects as we promised */
	delete decoder;
	delete input;
	delete info;
}

void
Player::play()
{
	stop();

	pthread_create(&playerThread, NULL, player_wrapper, this);
	havePlayerThread = true;
}

void
Player::stop()
{
	if (!havePlayerThread)
		return;

	/* Ask the decoder thread to terminate, and wait until it is gone */
	decoder->terminate();
	cont();
	pthread_join(playerThread, NULL);

	Input* oldInput = input; Info* oldInfo = info; Decoder* oldDecoder = decoder;

	input = NULL; info = NULL; decoder = NULL;

	delete oldInput;
	delete oldInfo;
	delete oldDecoder;

	havePlayerThread = false;
}

void
Player::pause()
{
	if (playerPaused || !havePlayerThread)
		return;

	pthread_suspend_np(playerThread);
	playerPaused = true;
}

void
Player::cont()
{
	if (!playerPaused || !havePlayerThread)
		return;

	pthread_resume_np(playerThread);
	playerPaused = false;
}

unsigned int
Player::getPlayingTime()
{
	if (decoder == NULL)
		return 0;
	return decoder->getPlayingTime();
}

unsigned int
Player::getTotalTime()
{
	if (decoder == NULL)
		return 0;
	return decoder->getTotalTime();
}
