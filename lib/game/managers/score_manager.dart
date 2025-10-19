import 'package:shared_preferences/shared_preferences.dart';
import 'package:bomber_game/constants/game_constants.dart';

class ScoreManager {
  int currentScore = 0;
  int highScore = 0;

  ScoreManager();

  Future<void> loadHighScore() async {
    final prefs = await SharedPreferences.getInstance();
    highScore = prefs.getInt(GameConstants.highScoreKey) ?? 0;
  }

  Future<void> saveHighScore() async {
    if (currentScore > highScore) {
      highScore = currentScore;
      final prefs = await SharedPreferences.getInstance();
      await prefs.setInt(GameConstants.highScoreKey, highScore);
    }
  }

  void addBlockScore(int level) {
    currentScore += GameConstants.pointsPerBlock * level;
  }

  void addLevelBonus(int level) {
    currentScore += GameConstants.levelCompleteBonus * level;
  }

  void reset() {
    currentScore = 0;
  }

  int get score => currentScore;
  int get high => highScore;
}
