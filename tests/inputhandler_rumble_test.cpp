// SPDX-License-Identifier: GPL-2.0-only

#include "controllermanager.h"
#include "sdlcontroller.h"

#include <QCoreApplication>
#include <QTemporaryDir>
#include <QTest>

class InputHandlerRumbleTest : public QObject {
    Q_OBJECT

    struct RumbleState {
        Uint16 low = 0;
        Uint16 high = 0;
    };

    static SDL_JoystickID attachGamepad(const char *name, Uint16 productId, RumbleState *state)
    {
        SDL_VirtualJoystickDesc desc;
        SDL_INIT_INTERFACE(&desc);
        desc.type = SDL_JOYSTICK_TYPE_GAMEPAD;
        desc.name = name;
        desc.vendor_id = 0x1234;
        desc.product_id = productId;
        desc.nbuttons = SDL_GAMEPAD_BUTTON_COUNT;
        desc.naxes = SDL_GAMEPAD_AXIS_COUNT;
        desc.button_mask = (1u << SDL_GAMEPAD_BUTTON_COUNT) - 1;
        desc.axis_mask = (1u << SDL_GAMEPAD_AXIS_COUNT) - 1;
        desc.userdata = state;
        desc.Rumble = [](void *userdata, Uint16 low, Uint16 high) {
            auto *state = static_cast<RumbleState *>(userdata);
            state->low = low;
            state->high = high;
            return true;
        };
        return SDL_AttachVirtualJoystick(&desc);
    }

    static void pressHome(SdlController &controller, SDL_JoystickID id)
    {
        SDL_Event event{};
        event.type = SDL_EVENT_GAMEPAD_BUTTON_DOWN;
        event.gbutton.which = id;
        event.gbutton.button = SDL_GAMEPAD_BUTTON_GUIDE;
        event.gbutton.down = true;
        QVERIFY(SDL_PushEvent(&event));
        event.type = SDL_EVENT_GAMEPAD_BUTTON_UP;
        event.gbutton.down = false;
        QVERIFY(SDL_PushEvent(&event));
        QVERIFY(QMetaObject::invokeMethod(&controller, "poll", Qt::DirectConnection));
    }

private Q_SLOTS:
    void routingAndPolicy()
    {
        RumbleState first;
        RumbleState second;
        SdlController controller;
        controller.setAutoSuppressInput(false);
        controller.setSuppressInput(false);
        auto &manager = ControllerManager::instance();
        manager.setEnabled(true);
        manager.setGameControllerEnabled(true);

        const auto firstId = attachGamepad("Rumble test 1", 1, &first);
        const auto secondId = attachGamepad("Rumble test 2", 2, &second);
        QVERIFY(firstId != 0);
        QVERIFY(secondId != 0);
        QVERIFY(QMetaObject::invokeMethod(&controller, "poll", Qt::DirectConnection));

        // No accepted gamepad input yet: startup focus changes must not rumble.
        QVERIFY(!controller.sendRumble(1000, 2000, 60));
        pressHome(controller, firstId);
        QVERIFY(manager.msSinceLastGamepadInput() < 3000);
        QVERIFY(controller.sendRumble(1000, 2000, 60));
        QCOMPARE(first.low, 1000);
        QCOMPARE(first.high, 2000);
        QCOMPARE(second.high, 0);

        // Axis input on a different controller must redirect the feedback.
        SDL_Event event{};
        event.type = SDL_EVENT_GAMEPAD_AXIS_MOTION;
        event.gaxis.which = secondId;
        event.gaxis.axis = SDL_GAMEPAD_AXIS_LEFTX;
        event.gaxis.value = 20000;
        QVERIFY(SDL_PushEvent(&event));
        QVERIFY(QMetaObject::invokeMethod(&controller, "poll", Qt::DirectConnection));
        QVERIFY(controller.sendRumble(3000, 4000, 60));
        QCOMPARE(second.low, 3000);
        QCOMPARE(second.high, 4000);

        controller.setSuppressInput(true);
        QVERIFY(!controller.sendRumble(1000, 2000, 60));
        controller.setSuppressInput(false);
        manager.setEnabled(false);
        QVERIFY(!controller.sendRumble(1000, 2000, 60));
        manager.setEnabled(true);
        manager.setGameControllerEnabled(false);
        QVERIFY(!controller.sendRumble(1000, 2000, 60));
        manager.setGameControllerEnabled(true);

        QString identifier;
        for (const auto &entry : manager.connectedControllers()) {
            const auto device = entry.toMap();
            if (device.value(QStringLiteral("name")).toString() == QStringLiteral("Rumble test 2")) {
                identifier = device.value(QStringLiteral("uniqueIdentifier")).toString();
            }
        }
        QVERIFY(!identifier.isEmpty());
        manager.setControllerEnabled(identifier, false);
        QVERIFY(!controller.sendRumble(1000, 2000, 60));
        manager.setControllerEnabled(identifier, true);
        QVERIFY(controller.sendRumble(5000, 6000, 60));

        // Unplugging the active pad must not transfer rumble to an unused pad.
        QVERIFY(SDL_DetachVirtualJoystick(secondId));
        QVERIFY(QMetaObject::invokeMethod(&controller, "poll", Qt::DirectConnection));
        QVERIFY(!controller.sendRumble(1000, 2000, 60));
        pressHome(controller, firstId);
        QVERIFY(controller.sendRumble(7000, 8000, 60));
        QCOMPARE(first.high, 8000);

        QTest::qWait(3100);
        QVERIFY(!controller.sendRumble(1000, 2000, 60));
        QVERIFY(SDL_DetachVirtualJoystick(firstId));
        QVERIFY(QMetaObject::invokeMethod(&controller, "poll", Qt::DirectConnection));
        QVERIFY(!controller.sendRumble(1000, 2000, 60));
    }
};

int main(int argc, char **argv)
{
    // Never access the user's controller settings or remote-desktop portal.
    QTemporaryDir config;
    if (!config.isValid()) {
        return 1;
    }
    qputenv("XDG_CONFIG_HOME", config.path().toUtf8());
    qputenv("DBUS_SESSION_BUS_ADDRESS", "unix:path=/nonexistent-inputhandler-test-bus");
    QCoreApplication app(argc, argv);
    InputHandlerRumbleTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "inputhandler_rumble_test.moc"
