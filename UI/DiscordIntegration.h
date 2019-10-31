#pragma once

#include <string>

// Simple wrapper around the Discord api.

// All platforms should call it, but we only actually take action on
// platforms where we want it enabled (only PC initially).

// All you need to call is FrameCallback, Shutdown, and UpdatePresence.

#define DISCORD_PRESENCE_MAX 129 // Discord has maximum of 128 bytes including \0 for presence buffers

class Discord {
public:
	Discord();
	~Discord();
	void Update();  // Call every frame or at least regularly. Will initialize if necessary.
	void Shutdown();

	void SetPresenceGame(const char *gameTitle, bool resetTimer = true);
	void SetPresenceMenu();
	void ClearPresence();

	void Connect();
	void Reconnect();
	bool IsFirstConnection();

private:
	void Init();
	bool IsEnabled() const;

	bool initialized_ = false;
	volatile bool isFirstConnection = true;
	char lastGameLoaded[DISCORD_PRESENCE_MAX];
	time_t lastTimeLoaded;
};

extern Discord g_Discord;
