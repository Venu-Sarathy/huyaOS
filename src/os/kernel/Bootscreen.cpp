#include <lib/graphic/Colors.h>
#include <devices/graphics/text/fonts/Fonts.h>
#include <BuildConfig.h>
#include <lib/file/bmp/Bmp.h>
#include "Bootscreen.h"
#include "Kernel.h"

auto versionString = String::format("hhuOS %s - git %s", BuildConfig::VERSION, BuildConfig::GIT_REV);
auto buildDate = String::format("Build date: %s", BuildConfig::BUILD_DATE);

void Bootscreen::update(uint8_t percentage, const String &message) {

    if (!isSplashActive) {

        log.info(message);

        return;
    }

    auto normalizedPercentage = static_cast<uint8_t>((percentage * 60) / 100);

    lfb->fillRect(0, 0, lfb->getResX(), lfb->getResY(), Colors::HHU_DARK_BLUE);

    lfb->placeString(*font, 50, 10, static_cast<char *>(versionString), Colors::HHU_GRAY, Colors::INVISIBLE);
    lfb->placeString(*font, 50, 15, static_cast<char *>(buildDate), Colors::HHU_GRAY, Colors::INVISIBLE);

    logo->draw(static_cast<uint16_t>((lfb->getResX() - logo->getWidth()) / 2),
               static_cast<uint16_t>((lfb->getResY() - logo->getHeight()) / 2));

    lfb->placeFilledRect(20, 85, 60, 2, Colors::HHU_BLUE_30);
    lfb->placeFilledCircle(20, 86, 1, Colors::HHU_BLUE_30);
    lfb->placeFilledCircle(80, 86, 1, Colors::HHU_BLUE_30);

    lfb->placeFilledRect(20, 85, normalizedPercentage, 2, Colors::HHU_BLUE);
    lfb->placeFilledCircle(20, 86, 1, Colors::HHU_BLUE);
    lfb->placeFilledCircle(static_cast<uint16_t>(20 + normalizedPercentage), 86, 1, Colors::HHU_BLUE);

    lfb->placeString(*font, 50, 90, (char*) message, Colors::HHU_GRAY, Colors::INVISIBLE);

    lfb->show();
}

void Bootscreen::init(uint16_t xres, uint16_t yres, uint8_t bpp) {

    if (isSplashActive) {

        lfb->init(xres, yres, bpp);

        lfb->enableDoubleBuffering();

        logoFile = File::open("/os/boot-logo.bmp", "r");

        if (logoFile != nullptr) {
            logo = new Bmp(logoFile);

            double scaling = (static_cast<double>(yres) / 600);

            if (scaling >= 1) {
                if(scaling - static_cast<uint8_t>(scaling) >= 0.8) {
                    scaling++;
                }

                logo->scaleUp(static_cast<uint8_t>(scaling));
            } else {
                scaling = 1 / scaling;

                if(scaling - static_cast<uint8_t>(scaling) >= 0.2) {
                    scaling++;
                }

                logo->scaleDown(static_cast<uint8_t>(scaling));
            }
        }
    }
}

void Bootscreen::finish() {

    if (isSplashActive) {

        lfb->disableDoubleBuffering();

        lfb->clear();

        if(logoFile != nullptr) {
            delete logoFile;
        }

        if(logo != nullptr) {
            delete logo;
        }
    }
}

Bootscreen::Bootscreen(bool showSplash, Logger &logger) : log(logger), isSplashActive(showSplash) {

    lfb = Kernel::getService<GraphicsService>()->getLinearFrameBuffer();
}
