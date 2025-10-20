import 'dart:typed_data';
import 'dart:math';
import 'package:audioplayers/audioplayers.dart';

enum GameVersion { bomber1982, bomber2025 }

class SoundManager {
  static final SoundManager _instance = SoundManager._internal();
  factory SoundManager() => _instance;
  SoundManager._internal();

  final AudioPlayer _dropPlayer = AudioPlayer();
  final AudioPlayer _hitPlayer = AudioPlayer();
  final AudioPlayer _affirmationPlayer = AudioPlayer();
  bool _soundEnabled = true;
  GameVersion _currentVersion = GameVersion.bomber1982;

  void toggleSound() {
    _soundEnabled = !_soundEnabled;
  }

  bool get isSoundEnabled => _soundEnabled;

  void setVersion(GameVersion version) {
    _currentVersion = version;
  }

  // Generate simple tone with WAV header
  Uint8List _generateTone(double frequency, double duration, {bool descending = false}) {
    const int sampleRate = 22050;
    const int numChannels = 1; // Mono
    const int bitsPerSample = 16;
    final int numSamples = (sampleRate * duration).toInt();
    final int dataSize = numSamples * numChannels * (bitsPerSample ~/ 8);
    final int fileSize = 44 + dataSize; // WAV header is 44 bytes

    final bytes = Uint8List(fileSize);
    final byteData = ByteData.view(bytes.buffer);

    // WAV Header
    // RIFF chunk
    bytes[0] = 0x52; bytes[1] = 0x49; bytes[2] = 0x46; bytes[3] = 0x46; // "RIFF"
    byteData.setUint32(4, fileSize - 8, Endian.little); // File size - 8
    bytes[8] = 0x57; bytes[9] = 0x41; bytes[10] = 0x56; bytes[11] = 0x45; // "WAVE"

    // fmt chunk
    bytes[12] = 0x66; bytes[13] = 0x6D; bytes[14] = 0x74; bytes[15] = 0x20; // "fmt "
    byteData.setUint32(16, 16, Endian.little); // fmt chunk size
    byteData.setUint16(20, 1, Endian.little); // Audio format (1 = PCM)
    byteData.setUint16(22, numChannels, Endian.little); // Number of channels
    byteData.setUint32(24, sampleRate, Endian.little); // Sample rate
    byteData.setUint32(28, sampleRate * numChannels * (bitsPerSample ~/ 8), Endian.little); // Byte rate
    byteData.setUint16(32, numChannels * (bitsPerSample ~/ 8), Endian.little); // Block align
    byteData.setUint16(34, bitsPerSample, Endian.little); // Bits per sample

    // data chunk
    bytes[36] = 0x64; bytes[37] = 0x61; bytes[38] = 0x74; bytes[39] = 0x61; // "data"
    byteData.setUint32(40, dataSize, Endian.little); // Data size

    // Generate audio samples
    for (int i = 0; i < numSamples; i++) {
      double freq = frequency;
      if (descending) {
        // Descending tone (whistling down)
        freq = frequency * (1.0 - (i / numSamples) * 0.7);
      }

      final t = i / sampleRate;
      final value = (sin(2 * pi * freq * t) * 32767 * 0.3).toInt(); // 30% volume

      // Write 16-bit little-endian sample at offset 44 + (i * 2)
      final offset = 44 + (i * 2);
      byteData.setInt16(offset, value, Endian.little);
    }

    return bytes;
  }

  // Play descending whistle for Equity Seed drop
  Future<void> playDropSound() async {
    if (!_soundEnabled) return;
    try {
      final bytes = _generateTone(800, 0.3, descending: true);
      await _dropPlayer.play(BytesSource(bytes));
    } catch (e) {
      // Silently fail if sound doesn't work
    }
  }

  // Generate white noise
  Uint8List _generateWhiteNoise(double duration) {
    const int sampleRate = 22050;
    const int numChannels = 1;
    const int bitsPerSample = 16;
    final int numSamples = (sampleRate * duration).toInt();
    final int dataSize = numSamples * numChannels * (bitsPerSample ~/ 8);
    final int fileSize = 44 + dataSize;

    final bytes = Uint8List(fileSize);
    final byteData = ByteData.view(bytes.buffer);
    final random = Random();

    // WAV Header (same as _generateTone)
    bytes[0] = 0x52; bytes[1] = 0x49; bytes[2] = 0x46; bytes[3] = 0x46;
    byteData.setUint32(4, fileSize - 8, Endian.little);
    bytes[8] = 0x57; bytes[9] = 0x41; bytes[10] = 0x56; bytes[11] = 0x45;
    bytes[12] = 0x66; bytes[13] = 0x6D; bytes[14] = 0x74; bytes[15] = 0x20;
    byteData.setUint32(16, 16, Endian.little);
    byteData.setUint16(20, 1, Endian.little);
    byteData.setUint16(22, numChannels, Endian.little);
    byteData.setUint32(24, sampleRate, Endian.little);
    byteData.setUint32(28, sampleRate * numChannels * (bitsPerSample ~/ 8), Endian.little);
    byteData.setUint16(32, numChannels * (bitsPerSample ~/ 8), Endian.little);
    byteData.setUint16(34, bitsPerSample, Endian.little);
    bytes[36] = 0x64; bytes[37] = 0x61; bytes[38] = 0x74; bytes[39] = 0x61;
    byteData.setUint32(40, dataSize, Endian.little);

    // Generate white noise samples
    for (int i = 0; i < numSamples; i++) {
      final value = (random.nextInt(65536) - 32768) * 0.2; // 20% volume
      final offset = 44 + (i * 2);
      byteData.setInt16(offset, value.toInt(), Endian.little);
    }

    return bytes;
  }

  // Play gentle bloop or white noise for hitting structure
  Future<void> playHitSound() async {
    if (!_soundEnabled) return;
    try {
      if (_currentVersion == GameVersion.bomber1982) {
        // White noise for 1982 version
        final bytes = _generateWhiteNoise(0.2);
        await _hitPlayer.play(BytesSource(bytes));
      } else {
        // Gentle bloop for 2025 version
        final bytes = _generateTone(400, 0.15);
        await _hitPlayer.play(BytesSource(bytes));
      }
    } catch (e) {
      // Silently fail
    }
  }

  // Play uplifting chime for affirmation
  Future<void> playAffirmationSound() async {
    if (!_soundEnabled) return;
    try {
      final bytes = _generateTone(600, 0.2);
      await _affirmationPlayer.play(BytesSource(bytes));
    } catch (e) {
      // Silently fail
    }
  }

  // Play bell for privilege check
  Future<void> playPrivilegeCheckSound() async {
    if (!_soundEnabled) return;
    try {
      final bytes = _generateTone(1000, 0.3);
      await _affirmationPlayer.play(BytesSource(bytes));
    } catch (e) {
      // Silently fail
    }
  }

  // Play compassionate tone for growth opportunity
  Future<void> playGameOverSound() async {
    if (!_soundEnabled) return;
    try {
      final bytes = _generateTone(300, 0.5);
      await _affirmationPlayer.play(BytesSource(bytes));
    } catch (e) {
      // Silently fail
    }
  }

  void dispose() {
    _dropPlayer.dispose();
    _hitPlayer.dispose();
    _affirmationPlayer.dispose();
  }
}
