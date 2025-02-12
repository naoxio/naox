#define ROCKS_CLAY_IMPLEMENTATION
#include "rocks.h"
#include <ctype.h>

enum {
    FONT_TITLE = 0,
    FONT_BODY = 1,
    FONT_COUNT
};

static uint16_t g_font_ids[FONT_COUNT];
static void* g_logo_image;
static void* g_project_images[6];

typedef struct {
    const char* title;
    const char* description;
    void* image;
    const char* link;
} Project;

static Project g_projects[] = {
    {
        .title = "Carousel",
        .description = "A beautifully crafted decision-making tool that brings joy to your choices. Spin the wheel to randomly select activities, make decisions, or add excitement to your daily routine. Perfect for teams, families, or personal use.",
        .image = NULL,
        .link = "https://carousel.waozi.xyz"
    },
    {
        .title = "Quest",
        .description = "Your personal journey companion. Track habits, organize tasks, and visualize your life progress through an intuitive interface. Features daily challenges, progress tracking, and beautiful data visualization to help you achieve your goals.",
        .image = NULL,
        .link = "http://quest.naox.io/"
    },
    {
        .title = "Inner Breeze",
        .description = "A comprehensive wellness app combining guided meditation, yoga, and breathing exercises. Features custom Wim Hof breathing sessions, guided meditations, and personalized yoga routines for all skill levels.",
        .image = NULL,
        .link = "https://inbreeze.xyz"
    },
    {
        .title = "Traveltint",
        .description = "Interactive world map journey tracker. Pin your adventures, share stories, and visualize your global footprint. Create beautiful travel timelines, track countries visited, and set exploration goals.",
        .image = NULL,
        .link = "https://traveltint.xyz"
    },
    {
        .title = "Lyra",
        .description = "Revolutionary identity management platform using Nostr protocol. Securely manage your online presence, login across platforms with one identity, and control your digital footprint with ease.",
        .image = NULL,
        .link = "https://lyra.xyz"
    },
    {
        .title = "Rocks",
        .description = "A powerful framework built on Clay, designed for developers who want flexibility in rendering and component management. Includes pre-built components and easy renderer switching for rapid application development.",
        .image = NULL,
        .link = "https://github.com/naoxio/rocks"
    }
};

char* get_image_filename(const char* project_name, char* buffer, size_t buffer_size) {
    int j = 0;
    for (int i = 0; project_name[i] && j < buffer_size - 1; i++) {
        if (project_name[i] == ' ') {
            buffer[j++] = '_';
        } else {
            buffer[j++] = tolower(project_name[i]);
        }
    }
    buffer[j] = '\0';
    return buffer;
}
static void render_project_card(Rocks* rocks, Project project) {
    Rocks_Theme theme = Rocks_GetTheme(rocks);
    Clay_String title = { .chars = project.title, .length = strlen(project.title) };
    Clay_String description = { .chars = project.description, .length = strlen(project.description) };
    Clay_Dimensions image_dims = Rocks_GetImageDimensions(rocks, project.image);
    
    CLAY({
        .layout = {
            .sizing = { CLAY_SIZING_FIXED(400), CLAY_SIZING_FIXED(300) },
            .padding = CLAY_PADDING_ALL(25),
            .childGap = 20,
            .layoutDirection = CLAY_TOP_TO_BOTTOM
        },
        .backgroundColor = theme.secondary,
        .cornerRadius = CLAY_CORNER_RADIUS(12),
        .border = {
            .color = theme.primary,
            .width = { 2, 2, 2, 2 }
        }
    }) {
        // Project image
        CLAY({
            .layout = {
                .sizing = { CLAY_SIZING_FIXED(350), CLAY_SIZING_FIXED(140) }
            },
            .image = {
                .imageData = project.image,
                .sourceDimensions = image_dims
            }
        }) {}

        CLAY_TEXT(title, CLAY_TEXT_CONFIG({
            .textColor = theme.primary,
            .fontSize = 28,
            .fontId = g_font_ids[FONT_TITLE]
        }));

        CLAY_TEXT(description, CLAY_TEXT_CONFIG({
            .textColor = theme.text,
            .fontSize = 16,
            .fontId = g_font_ids[FONT_BODY]
        }));

        CLAY({
            .layout = {
                .sizing = { CLAY_SIZING_FIXED(150), CLAY_SIZING_FIXED(45) },
                .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = Clay_Hovered() ? theme.primary_hover : theme.primary,
            .cornerRadius = CLAY_CORNER_RADIUS(8)
        }) {
            CLAY_TEXT(CLAY_STRING("Explore"), CLAY_TEXT_CONFIG({
                .textColor = theme.secondary,
                .fontSize = 18,
                .fontId = g_font_ids[FONT_BODY]
            }));
        }
    }
}

static Clay_RenderCommandArray app_update(Rocks* rocks, float dt) {
    Rocks_Theme theme = Rocks_GetTheme(rocks);
    Clay_Dimensions logo_dims = Rocks_GetImageDimensions(rocks, g_logo_image);
    
    Clay_BeginLayout();
    
    CLAY({
        .layout = {
            .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .childGap = 60,
            .padding = CLAY_PADDING_ALL(40)
        },
        .scroll = { .vertical = true, .horizontal = false },
        .backgroundColor = theme.background
    }) {
        // Logo section
        CLAY({
            .layout = {
                .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(200) },
                .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER }
            }
        }) {
            CLAY({
                .layout = {
                    .sizing = { CLAY_SIZING_FIXED(180), CLAY_SIZING_FIXED(180) }
                },
                .image = {
                    .imageData = g_logo_image,
                    .sourceDimensions = logo_dims
                }
            }) {}
        }

        // Projects grid
        CLAY({
            .layout = {
                .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .childGap = 30,
                .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_TOP }
            }
        }) {
            for (int i = 0; i < sizeof(g_projects)/sizeof(g_projects[0]); i++) {
                render_project_card(rocks, g_projects[i]);
            }
        }
    }
    
    return Clay_EndLayout();
}
int main(void) {
    Rocks_Config config = {
        .window_width = 1440,
        .window_height = 900,
        .window_title = "NaoX",
        .theme = {
            .background = (Clay_Color){12, 12, 20, 255},      // Dark blue-black
            .primary = (Clay_Color){0, 255, 255, 255},        // Cyan
            .primary_hover = (Clay_Color){0, 200, 200, 255},  // Darker cyan
            .secondary = (Clay_Color){20, 24, 36, 255},       // Dark blue-grey
            .text = (Clay_Color){180, 180, 190, 255},         // Light grey
            .text_secondary = (Clay_Color){140, 140, 150, 255},
            .border = (Clay_Color){0, 180, 180, 255},         // Dimmed cyan
            .scrollbar_track = (Clay_Color){20, 24, 36, 200},
            .scrollbar_thumb = (Clay_Color){0, 180, 180, 255},
            .scrollbar_thumb_hover = (Clay_Color){0, 200, 200, 255}
        }
    };

    #ifdef ROCKS_USE_RAYLIB
    Rocks_RaylibConfig raylib_config = {
        .screen_width = 1440,
        .screen_height = 900
    };
    config.renderer_config = &raylib_config;
    #endif

    Rocks* rocks = Rocks_Init(config);
    if (!rocks) return 1;

    // Load fonts
    g_font_ids[FONT_TITLE] = Rocks_LoadFont("assets/Roboto-Bold.ttf", 24, FONT_TITLE);
    g_font_ids[FONT_BODY] = Rocks_LoadFont("assets/Roboto-Regular.ttf", 16, FONT_BODY);

    // Load images
    g_logo_image = Rocks_LoadImage(rocks, "assets/logo.png");

    for (int i = 0; i < 6; i++) {
        char filename[64];
        char path[128];
        get_image_filename(g_projects[i].title, filename, sizeof(filename));
        snprintf(path, sizeof(path), "assets/projects/%s.png", filename);
        printf("Loading image: %s\n", path);
        g_project_images[i] = Rocks_LoadImage(rocks, path);
        g_projects[i].image = g_project_images[i];
    }

    Rocks_Run(rocks, app_update);
    
    // Cleanup
    Rocks_UnloadFont(g_font_ids[FONT_TITLE]);
    Rocks_UnloadFont(g_font_ids[FONT_BODY]);
    Rocks_UnloadImage(rocks, g_logo_image);
    for (int i = 0; i < 6; i++) {
        Rocks_UnloadImage(rocks, g_project_images[i]);
    }
    Rocks_Cleanup(rocks);
    
    return 0;
}