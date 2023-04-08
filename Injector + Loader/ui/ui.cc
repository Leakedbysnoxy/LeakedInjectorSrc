#include "ui.hh"
#include "../globals.hh"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../auth/auth.hpp"
#include "../streaming/stream.h"
#include "../inject/injectorapi/drvutils.h"
#include "../inject/inject/injector.h"
#include "../streaming/blowfish.h"

#pragma comment(lib, "wininet.lib")
#pragma warning(disable : 4996)
#define sk(x) skCrypt(x).decrypt()

using namespace KeyAuth;
static int currentColumn = 1;

std::string name = ""; // application name. right above the blurred text aka the secret on the licenses tab among other tabs
std::string ownerid = ""; // ownerid, found in account settings. click your profile picture on top right of dashboard and then account settings.
std::string secret = ""; // app secret, the blurred text on licenses tab and other tabs
std::string version = "1.0"; // leave alone unless you've changed version on website
std::string url = "https://keyauth.win/api/1.2/"; // change if you're self-hosting

api KeyAuthApp(name, ownerid, secret, version, url);

BOOL SaveLoginInformation(std::string username)
{
	std::string EncryptedUsername, EncryptedPassword;

	for (int i = 0; (i < 100 && username[i] != '\0'); i++) username[i] = username[i] + 2; EncryptedUsername = username;

	std::basic_ofstream<char, std::char_traits<char>> AuthConfig("C:\\Windows\\Temp\\savedauth1.bin");

	if (AuthConfig.is_open() || !AuthConfig.fail() || !AuthConfig.bad())
	{
		AuthConfig << EncryptedUsername << std::endl;
		return TRUE;
	}
	else return FALSE;
}

BOOL LoginFromFile()
{
	INT CurrentRow = 0;
	std::basic_string  <char, std::char_traits<char>, std::allocator<char>> CurrentLine;
	std::basic_ifstream<char, std::char_traits<char>> AuthConfig("C:\\Windows\\Temp\\savedauth1.bin");

	if (AuthConfig.is_open() || !AuthConfig.fail() || !AuthConfig.bad())
	{
		std::string key;

		while (std::getline(AuthConfig, CurrentLine)) {

			if (CurrentRow == 0) { key = CurrentLine; }
			CurrentRow += 1;
		}

		for (int i = 0; (i < 100 && key[i] != '\0'); i++) key[i] = key[i] - 2;

		std::strncpy(globals.key, key.c_str(), sizeof(globals.key));

		return TRUE;
	}
	else return FALSE;
}

void ColoredText(const char* Label, bool center, ImVec4 color)
{
	if (center)
	{
		auto windowWidth = ImGui::GetWindowSize().x;
		auto textWidth = ImGui::CalcTextSize(Label).x;
		ImGui::SetCursorPos({ (windowWidth - textWidth) * 0.5f, 62 });
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, color);
		ImGui::Text(Label);
		ImGui::PopStyleColor();
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, color);
		ImGui::Text(Label);
		ImGui::PopStyleColor();
	}
}

void ui::render() {
    if (!globals.active) return;

    ImGui::SetNextWindowPos(ImVec2(window_pos.x, window_pos.y), ImGuiCond_Once);
    ImGui::SetNextWindowBgAlpha(1.0f);

    ImGui::Begin(window_title, &globals.active, window_flags);
	{
		switch (currentColumn)
		{
		case 1:
			ImGui::SetCursorPos(ImVec2(45, 55));
			ImGui::InputTextWithHint("##KeyInput", "              Enter License Here", globals.key, IM_ARRAYSIZE(globals.key));
			ImGui::SetCursorPos(ImVec2(85, 90));
			if (ImGui::Button(("Login"), ImVec2(80, 20))) {
				KeyAuthApp.license(globals.key);
				if (!KeyAuthApp.data.success)
				{
					if (std::filesystem::exists("C:\\Windows\\Temp\\savedauth1.bin"))
					{
						remove("C:\\Windows\\Temp\\savedauth1.bin");
					}
					currentColumn = 3;
				}
				else {
					SaveLoginInformation(globals.key);
					HWND game = FindWindow(NULL, xor_w(L"Call of Duty® HQ"));
					if (game)
					{
						start_driver();
						std::string decryptedDLL = GetDecryptedDLL();
						if (!Kernel_Injector((L"Call of Duty® HQ"), (BYTE*)decryptedDLL.c_str()))
						{
							currentColumn = 2;
						}
						currentColumn = 4;
					}
					else
					{
						currentColumn = 2;
					}
				}
			}
			break;
		case 2:
			ColoredText("Failed To Inject", true, ImVec4(255, 0, 0, 255));
			ImGui::SetCursorPos(ImVec2(90, 85));
			if (ImGui::Button(("Exit"), ImVec2(70, 20)))
			{
				exit(-1);
			}
			break;
		case 3:
			ColoredText(KeyAuthApp.data.message.c_str(), true, ImVec4(255, 0, 0, 255));
			ImGui::SetCursorPos(ImVec2(90, 85));
			if (ImGui::Button(("Exit"), ImVec2(70, 20)))
			{
				exit(-1);
			}
			break;
		case 4:
			ColoredText("Successfully Injected", true, ImVec4(0, 255, 0, 255));
			ImGui::SetCursorPos(ImVec2(90, 85));
			if (ImGui::Button(("Exit"), ImVec2(70, 20)))
			{
				exit(-1);
			}
			break;
		}
	}
    ImGui::End();
}

void ui::init(LPDIRECT3DDEVICE9 device) {
    dev = device;
	
	start_driver();
	KeyAuthApp.init();

	LoginFromFile();

    ImGui::StyleColorsDark();
	ImGuiStyle& Style = ImGui::GetStyle();
	auto Color = Style.Colors;

	Style.WindowMinSize = ImVec2(250, 150);
	Style.WindowBorderSize = 0;

	Style.ChildRounding = 0;
	Style.FrameRounding = 5;
	Style.ScrollbarRounding = 0;
	Style.GrabRounding = 0;
	Style.PopupRounding = 0;
	Style.WindowRounding = 0;
	Style.WindowTitleAlign = ImVec2(0.5, 0.5);
	Style.ButtonTextAlign = ImVec2(0.5, 0.5);

	Color[ImGuiCol_WindowBg] = ImColor(10, 12, 15, 255);
	//Color[ImGuiCol_WindowBg] = ImColor(16, 16, 16, 255);

	Color[ImGuiCol_FrameBg] = ImColor(20, 20, 20, 255);
	Color[ImGuiCol_FrameBgActive] = ImColor(18, 17, 17, 255);
	Color[ImGuiCol_FrameBgHovered] = ImColor(18, 17, 17, 255);

	Color[ImGuiCol_Button] = ImColor(20, 20, 20, 255);
	Color[ImGuiCol_ButtonActive] = ImColor(32, 32, 32, 255);
	Color[ImGuiCol_ButtonHovered] = ImColor(32, 32, 32, 255);

	Color[ImGuiCol_Separator] = ImColor(36, 36, 36, 255);

	Color[ImGuiCol_TitleBg] = ImColor(16, 16, 16, 255);
	//Color[ImGuiCol_TitleBg] = ImColor(16, 16, 16, 255);
	Color[ImGuiCol_TitleBgActive] = ImColor(16, 16, 16, 255);

	Color[ImGuiCol_ResizeGrip] = ImColor(30, 30, 30, 255);
	Color[ImGuiCol_ResizeGripActive] = ImColor(30, 30, 30, 255);
	Color[ImGuiCol_ResizeGripHovered] = ImColor(30, 30, 30, 255);

	Color[ImGuiCol_ChildBg] = ImColor(0, 0, 0, 255);

	Color[ImGuiCol_ScrollbarBg] = ImColor(24, 24, 24, 255);
	Color[ImGuiCol_ScrollbarGrab] = ImColor(24, 24, 24, 255);
	Color[ImGuiCol_ScrollbarGrabActive] = ImColor(24, 24, 24, 255);
	Color[ImGuiCol_ScrollbarGrabActive] = ImColor(24, 24, 24, 255);

	Color[ImGuiCol_Header] = ImColor(18, 17, 17, 255);
	Color[ImGuiCol_HeaderActive] = ImColor(18, 17, 17, 255);
	Color[ImGuiCol_HeaderHovered] = ImColor(18, 17, 17, 255);
	Color[ImGuiCol_CheckMark] = ImColor(255, 255, 255, 255);

	if (window_pos.x == 0) {
		RECT screen_rect{};
		GetWindowRect(GetDesktopWindow(), &screen_rect);
		screen_res = ImVec2(float(screen_rect.right), float(screen_rect.bottom));
		window_pos = (screen_res - window_size) * 0.5f;
	}
}
