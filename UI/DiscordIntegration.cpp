
#include <ctime>
#include <cassert>

#include "ppsspp_config.h"
#include "Common/Log.h"
#include "Core/Config.h"
#include "DiscordIntegration.h"
#include "i18n/i18n.h"


// TODO: Enable on more platforms. Make optional.

Discord g_Discord;

static const char *ppsspp_app_id = "423397985041383434";

#ifdef ENABLE_DISCORD
// No context argument? What?
static void handleDiscordError(int errCode, const char *message) {
	ERROR_LOG(SYSTEM, "Discord error code %d: '%s'", errCode, message);
}

static void handleDiscordReady(const DiscordUser *request) {
	if (g_Discord.IsFirstConnection()) {
		g_Discord.Connect();
	}
	else {
		g_Discord.Reconnect();
	}
}
#endif

Discord::Discord() {
	memset(lastGameLoaded, 0x00, DISCORD_PRESENCE_MAX);
	lastTimeLoaded = time(0);
}

Discord::~Discord() {
	if (initialized_) {
		ERROR_LOG(SYSTEM, "Discord destructor running though g_Discord.Shutdown() has not been called.");
	}
}

bool Discord::IsEnabled() const {
	return g_Config.bDiscordPresence;
}

void Discord::Init() {
	assert(IsEnabled());
	assert(!initialized_);

#ifdef ENABLE_DISCORD
	DiscordEventHandlers eventHandlers{};
	eventHandlers.errored = &handleDiscordError;
	eventHandlers.ready = &handleDiscordReady;
	Discord_Initialize(ppsspp_app_id, &eventHandlers, 0, nullptr);
	INFO_LOG(SYSTEM, "Discord connection initialized");
#endif

	initialized_ = true;

#ifdef ENABLE_DISCORD
	if (strnlen(lastGameLoaded, DISCORD_PRESENCE_MAX)) {
		SetPresenceGame(lastGameLoaded, false);
	}
#endif
}

void Discord::Shutdown() {
	assert(initialized_);
#ifdef ENABLE_DISCORD
	Discord_Shutdown();
#endif
	initialized_ = false;
}

void Discord::Update() {
	if (!IsEnabled()) {
		if (initialized_) {
			Shutdown();
		}
		return;
	} else {
		if (!initialized_) {
			Init();
		}
	}

#ifdef ENABLE_DISCORD
#ifdef DISCORD_DISABLE_IO_THREAD
	Discord_UpdateConnection();
#endif
	Discord_RunCallbacks();
#endif
}

void Discord::SetPresenceGame(const char *gameTitle, bool resetTimer) {
	if (!IsEnabled())
		return;
	
	if (!initialized_) {
		Init();
	}

#ifdef ENABLE_DISCORD
	I18NCategory *sc = GetI18NCategory("Screen");

	DiscordRichPresence discordPresence{};
	snprintf(lastGameLoaded, DISCORD_PRESENCE_MAX, "%s", gameTitle);
	discordPresence.state = lastGameLoaded;
	std::string details = sc->T("Playing");
	discordPresence.details = details.c_str();
	if (resetTimer) {
		discordPresence.startTimestamp = time(0);
	}
	else {
		discordPresence.startTimestamp = lastTimeLoaded;
	}
	discordPresence.largeImageText = "PPSSPP is the best PlayStation Portable emulator around!";
#ifdef GOLD
	discordPresence.largeImageKey = "icon_gold_png";
#else
	discordPresence.largeImageKey = "icon_regular_png";
#endif
	Discord_UpdatePresence(&discordPresence);
#endif
}

void Discord::SetPresenceMenu() {
	if (!IsEnabled())
		return;

	if (!initialized_) {
		Init();
	}

#ifdef ENABLE_DISCORD
	I18NCategory *sc = GetI18NCategory("Screen");

	DiscordRichPresence discordPresence{};
	discordPresence.state = sc->T("In menu");
	discordPresence.details = "";
	discordPresence.startTimestamp = time(0);
	discordPresence.largeImageText = "PPSSPP is the best PlayStation Portable emulator around!";
#ifdef GOLD
	discordPresence.largeImageKey = "icon_gold_png";
#else
	discordPresence.largeImageKey = "icon_regular_png";
#endif
	Discord_UpdatePresence(&discordPresence);
#endif
}

void Discord::ClearPresence() {
	if (!IsEnabled() || !initialized_)
		return;

#ifdef ENABLE_DISCORD
	Discord_ClearPresence();
#endif
}

void Discord::Connect() {
	isFirstConnection = false;
}

void Discord::Reconnect() {
	SetPresenceGame(lastGameLoaded, false);
}

bool Discord::IsFirstConnection() {
	return isFirstConnection;
}

DiscordRichPresenceWrapper::DiscordRichPresenceWrapper() {
	storedPresence.state = (char*)calloc(DiscordRichPresenceWrapper::TEXT_SIZE, sizeof(char));
	storedPresence.details = (char*)calloc(DiscordRichPresenceWrapper::TEXT_SIZE, sizeof(char));
	storedPresence.largeImageKey = (char*)calloc(DiscordRichPresenceWrapper::KEY_SIZE, sizeof(char));
	storedPresence.largeImageText = (char*)calloc(DiscordRichPresenceWrapper::TEXT_SIZE, sizeof(char));
	storedPresence.smallImageKey = (char*)calloc(DiscordRichPresenceWrapper::KEY_SIZE, sizeof(char));
	storedPresence.smallImageText = (char*)calloc(DiscordRichPresenceWrapper::TEXT_SIZE, sizeof(char));
	storedPresence.partyId = (char*)calloc(DiscordRichPresenceWrapper::TEXT_SIZE, sizeof(char));
	storedPresence.matchSecret = (char*)calloc(DiscordRichPresenceWrapper::SECRET_SIZE, sizeof(char));
	storedPresence.joinSecret = (char*)calloc(DiscordRichPresenceWrapper::SECRET_SIZE, sizeof(char));
	storedPresence.spectateSecret = (char*)calloc(DiscordRichPresenceWrapper::SECRET_SIZE, sizeof(char));
}

DiscordRichPresenceWrapper::~DiscordRichPresenceWrapper() {
	free((void *)storedPresence.state);
	free((void *)storedPresence.details);
	free((void *)storedPresence.largeImageKey);
	free((void *)storedPresence.largeImageText);
	free((void *)storedPresence.smallImageKey);
	free((void *)storedPresence.smallImageText);
	free((void *)storedPresence.partyId);
	free((void *)storedPresence.matchSecret);
	free((void *)storedPresence.joinSecret);
	free((void *)storedPresence.spectateSecret);
}

void DiscordRichPresenceWrapper::GetStoredPresence(DiscordRichPresence *input) {
	input->state = storedPresence.state;
	input->details = storedPresence.details;
	input->startTimestamp = storedPresence.startTimestamp;
	input->endTimestamp = storedPresence.endTimestamp;
	input->largeImageKey = storedPresence.largeImageKey;
	input->largeImageText = storedPresence.largeImageText;
	input->smallImageKey = storedPresence.smallImageKey;
	input->smallImageText = storedPresence.smallImageText;
	input->partyId = storedPresence.partyId;
	input->partySize = storedPresence.partySize;
	input->partyMax = storedPresence.partyMax;
	input->matchSecret = storedPresence.matchSecret;
	input->joinSecret = storedPresence.joinSecret;
	input->spectateSecret = storedPresence.spectateSecret;
	input->instance = storedPresence.instance;
}

void DiscordRichPresenceWrapper::UpdateStoredPresence(DiscordRichPresence *input) {

	if (input->state != nullptr) {
		snprintf((char *)storedPresence.state,  DiscordRichPresenceWrapper::TEXT_SIZE, "%s", input->state);
	}

	if (input->details != nullptr) {
		snprintf((char *)storedPresence.details, DiscordRichPresenceWrapper::TEXT_SIZE, "%s", input->details);
	}

	storedPresence.startTimestamp = input->startTimestamp;
	storedPresence.endTimestamp = input->endTimestamp;

	if (input->largeImageKey != nullptr) {
		snprintf((char *)storedPresence.largeImageKey, DiscordRichPresenceWrapper::KEY_SIZE, "%s", input->largeImageKey);
	}

	if (input->largeImageText != nullptr) {
		snprintf((char *)storedPresence.largeImageText, DiscordRichPresenceWrapper::TEXT_SIZE, "%s", input->largeImageText);
	}

	if (input->smallImageKey != nullptr) {
		snprintf((char *)storedPresence.smallImageKey, DiscordRichPresenceWrapper::KEY_SIZE, "%s", input->smallImageKey);
	}

	if (input->smallImageText != nullptr) {
		snprintf((char *)storedPresence.smallImageText, DiscordRichPresenceWrapper::TEXT_SIZE, "%s", input->smallImageText);
	}

	if (input->partyId != nullptr) {
		snprintf((char *)storedPresence.partyId, DiscordRichPresenceWrapper::TEXT_SIZE, "%s", input->partyId);
	}

	storedPresence.partySize = input->partySize;
	storedPresence.partyMax = input->partyMax;

	if (input->matchSecret != nullptr) {
		snprintf((char *)storedPresence.matchSecret, DiscordRichPresenceWrapper::SECRET_SIZE, "%s", input->matchSecret);
	}

	if (input->joinSecret != nullptr) {
		snprintf((char *)storedPresence.joinSecret, DiscordRichPresenceWrapper::SECRET_SIZE, "%s", input->joinSecret);
	}

	if (input->spectateSecret != nullptr) {
		snprintf((char *)storedPresence.spectateSecret, DiscordRichPresenceWrapper::SECRET_SIZE, "%s", input->spectateSecret);
	}

	storedPresence.instance = input->instance;

}
