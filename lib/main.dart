import 'dart:io';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:flame/game.dart';
import 'package:window_manager/window_manager.dart';
import 'package:bomber_game/game/bomber_game.dart';

Future<void> main() async {
  WidgetsFlutterBinding.ensureInitialized();

  // Set window size for Linux
  if (Platform.isLinux || Platform.isWindows || Platform.isMacOS) {
    await windowManager.ensureInitialized();

    const windowOptions = WindowOptions(
      size: Size(800, 600),
      center: true,
      title: 'Bomber Game',
      minimumSize: Size(800, 600),
      maximumSize: Size(800, 600),
    );

    await windowManager.waitUntilReadyToShow(windowOptions, () async {
      await windowManager.show();
      await windowManager.focus();
    });
  }

  // For Android: Set system UI mode to show status bar
  if (Platform.isAndroid) {
    SystemChrome.setEnabledSystemUIMode(
      SystemUiMode.edgeToEdge,
      overlays: [SystemUiOverlay.top, SystemUiOverlay.bottom],
    );
  }

  runApp(const BomberGameApp());
}

class BomberGameApp extends StatelessWidget {
  const BomberGameApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Bomber Game',
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.blue),
        useMaterial3: true,
      ),
      home: Scaffold(
        body: SafeArea(
          child: GameWidget(game: BomberGame()),
        ),
      ),
    );
  }
}
