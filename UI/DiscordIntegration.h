#pragma once

#include <string>

// Simple wrapper around the Discord api.

// All platforms should call it, but we only actually take action on
// platforms where we want it enabled (only PC initially).

// All you need to call is FrameCallback, Shutdown, and UpdatePresence.

#if (PPSSPP_PLATFORM(WINDOWS) || PPSSPP_PLATFORM(MAC) || PPSSPP_PLATFORM(LINUX)) && !PPSSPP_PLATFORM(ANDROID) && !PPSSPP_PLATFORM(UWP)

#ifdef _MSC_VER
#define ENABLE_DISCORD
#elif USE_DISCORD
#define ENABLE_DISCORD
#endif

#else

// TODO

#endif

#ifdef ENABLE_DISCORD
#include "ext/discord-rpc/include/discord_rpc.h"
#endif

#define DISCORD_PRESENCE_MAX 129 // Discord has maximum of 128 bytes including \0 for presence buffers

class DiscordRichPresenceWrapper {
public:
	DiscordRichPresenceWrapper();
	~DiscordRichPresenceWrapper();
	void GetStoredPresence(DiscordRichPresence *input); // overrwrite input with storedPresence
	void UpdateStoredPresence(DiscordRichPresence *input); // overrwrite storedPresence with input

	const int TEXT_SIZE = 128;
	const int KEY_SIZE = 32;
	const int SECRET_SIZE = 128;

private:
	DiscordRichPresence storedPresence{};
};

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
	DiscordRichPresenceWrapper lastStoredPresence;
	char lastGameLoaded[DISCORD_PRESENCE_MAX];
	time_t lastTimeLoaded;
};

extern Discord g_Discord;
