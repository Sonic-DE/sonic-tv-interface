// SPDX-License-Identifier: GPL-2.0-only
/* Test envmanager config.h policy: maximizing placement, borderless-by-default,
 * disabled interactive move, and no Wayland groups.
 */
#include <QMap>
#include <QString>
#include <QTest>
#include <QVariant>

#include <KConfigGroup>
#include <KSharedConfig>

#include "config.h"

class EnvmanagerConfigTest : public QObject {
    Q_OBJECT

private:
    KSharedConfig::Ptr makeConfig(bool decorationsEnabled)
    {
        auto config = KSharedConfig::openConfig(QStringLiteral("test_kwinrc_%1").arg(decorationsEnabled ? QStringLiteral("decor") : QStringLiteral("nodecor")),
                                                KConfig::SimpleConfig);
        auto group = KConfigGroup{config, QStringLiteral("General")};
        group.writeEntry("windowDecorationsEnabled", decorationsEnabled);
        config->sync();
        return config;
    }

    void verifyCommonSettings(const QMap<QString, QMap<QString, QVariant>> &settings)
    {
        // Windows group must always have maximizing placement and disabled move
        QVERIFY(settings.contains("Windows"));
        QCOMPARE(settings["Windows"]["Placement"].toString(), "Maximizing");
        QCOMPARE(settings["Windows"]["InteractiveWindowMoveEnabled"].toBool(), false);

        // Plugins group preserved
        QVERIFY(settings.contains("Plugins"));
        QCOMPARE(settings["Plugins"]["blurEnabled"].toBool(), false);
        QCOMPARE(settings["Plugins"]["gamecontrollerEnabled"].toBool(), false);
        QCOMPARE(settings["Plugins"]["hidecursorEnabled"].toBool(), true);

        // Decoration group preserved
        QVERIFY(settings.contains("org.kde.kdecoration2"));
        QCOMPARE(settings["org.kde.kdecoration2"]["NoPlugin"].toBool(), false);

        // Input group preserved
        QVERIFY(settings.contains("Input"));
        QCOMPARE(settings["Input"]["TabletMode"].toString(), "off");

        // Cursor effect group preserved
        QVERIFY(settings.contains("Effect-hidecursor"));
        QCOMPARE(settings["Effect-hidecursor"]["HideOnTyping"].toBool(), true);
        QCOMPARE(settings["Effect-hidecursor"]["InactivityDuration"].toInt(), 5);

        // No Wayland group
        QVERIFY(!settings.contains("Wayland"));
    }

private Q_SLOTS:
    void testDefaultSettingsEmpty()
    {
        QVERIFY(KWINRC_DEFAULT_SETTINGS.isEmpty());
    }

    void testSettingsWithDecorationsDisabled()
    {
        auto config = makeConfig(false);
        auto settings = getKwinrcSettings(config);

        // BorderlessMaximizedWindows should be true when decorations are disabled
        QCOMPARE(settings["Windows"]["BorderlessMaximizedWindows"].toBool(), true);
        verifyCommonSettings(settings);
    }

    void testSettingsWithDecorationsEnabled()
    {
        auto config = makeConfig(true);
        auto settings = getKwinrcSettings(config);

        // BorderlessMaximizedWindows should be false when decorations are enabled
        QCOMPARE(settings["Windows"]["BorderlessMaximizedWindows"].toBool(), false);
        verifyCommonSettings(settings);
    }
};

QTEST_GUILESS_MAIN(EnvmanagerConfigTest)
#include "envmanager_config_test.moc"
