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

	void SetPresenceGame(const char *gameTitle);
	void SetPresenceMenu();
	void ClearPresence();

private:
	void Init();
	bool IsEnabled() const;

	bool initialized_ = false;
	char lastGameLoaded[DISCORD_PRESENCE_MAX];
};

extern Discord g_Discord;
