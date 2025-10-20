import 'dart:io';
import 'package:flutter/material.dart';
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
        body: GameWidget(game: BomberGame()),
      ),
    );
  }
}
