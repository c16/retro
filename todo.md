# Bomber Game - Next Iteration TODO

## Two Versions

### Bomber-1982 (Classic Retro Version) ✅
Current implementation - classic VIC-20 style arcade game

### Bomber-2025 (Contemporary Reimagining)
Ultra-progressive, hyper-inclusive modern interpretation

---

## Bomber-1982 Enhancements

### Sound Effects
- [ ] Implement whistling sound for bomb drop (find GStreamer alternative or simple tone generator)
- [ ] Add white noise/explosion sound for building destruction
- [ ] Plane engine ambient sound
- [ ] Level complete jingle
- [ ] Game over sound

### Gameplay Polish
- [ ] Add particle effects for explosions (simple colored pixels)
- [ ] Screen shake on building destruction
- [ ] Smoother altitude transitions
- [ ] Implement proper character-grid movement
- [ ] Add "Ready Player One" style retro CRT scanline effect option

### Additional Features
- [ ] Multiplayer mode (2 players alternating turns)
- [ ] Difficulty settings (Easy/Normal/Hard)
- [ ] Pause menu with options
- [ ] Statistics screen (accuracy, buildings destroyed, etc.)
- [ ] Achievement system (retro badges)

---

## Bomber-2025 Features

### Core Concept Changes
**"Healing Harmony 2025: A Journey of Collective Restoration"**

### Renamed Elements
- ~~Bomber~~ → "Aerial Sustainability Coordinator"
- ~~Plane~~ → "Carbon-Neutral Renewable Energy Delivery Vehicle"
- ~~Bombs~~ → "Equity Seeds" or "Justice Parcels"
- ~~Buildings~~ → "Problematic Structures of Historical Oppression"
- ~~Destruction~~ → "Decolonization" or "Restorative Reclamation"
- ~~Score~~ → "Social Justice Impact Points"
- ~~Lives~~ → "Opportunities for Growth and Learning"

### Visual Changes
- [ ] Replace plane with solar-powered, zero-emission aircraft
- [ ] Buildings should be labeled with microaggressions: "Gender Binary Enforcement Center", "Fossil Fuel Headquarters", "Patriarchal Institution", "Colonizer Monument"
- [ ] Rainbow gradient sky instead of blue (celebrating LGBTQIA2S+ pride)
- [ ] "Equity Seeds" should be heart-shaped and rainbow colored
- [ ] When buildings are "healed", they transform into:
  - Community gardens
  - Safe spaces
  - Pronoun education centers
  - Emotional support animal sanctuaries
  - Trigger warning distribution hubs

### Gameplay Mechanics
- [ ] **Trigger Warning System**: Before each level, display comprehensive content warnings
  - "This level contains structures built without consent from the land"
  - "Some buildings may represent capitalist ideologies"
  - "Warning: This game was made on stolen indigenous land"

- [ ] **Consent-Based Bombing**: Must get permission before each "Equity Seed" drop
  - Pop-up: "Do you consent to receiving this healing energy?"
  - Buildings can decline (50% chance)
  - If declined, must respect boundary and try different building

- [ ] **Privilege Check System**:
  - Plane's fuel is "Privilege Points"
  - Must constantly acknowledge privilege to continue
  - Random "Check Your Privilege" pop-ups pause game
  - Failure to acknowledge = lose a turn

- [ ] **Microaggression Meter**:
  - Flying too fast = "rushing through important conversations"
  - Flying too slow = "not doing the work"
  - Dropping too many seeds = "performative allyship"
  - Not dropping enough = "complicity through inaction"

- [ ] **Intersectionality Scoring Multiplier**:
  - Bonus points for hitting multiple oppression types simultaneously
  - "You've decolonized a heteronormative structure AND a capitalist monument! 2x multiplier!"

### UI/UX Changes
- [ ] **Pronoun Selection Screen** (mandatory, 127 options minimum)
  - Player must select pronouns for:
    - Self
    - The plane
    - Each individual Equity Seed
    - The sky
    - The score display
  - Option to change pronouns mid-game (with celebration animation)

- [ ] **Land Acknowledgment** (unskippable, every time game starts)
  - 3-minute scrolling text acknowledging every indigenous nation
  - Must read aloud option with voice recognition
  - Quiz after to ensure genuine engagement

- [ ] **Emotional Support Mode**:
  - If player is struggling, game offers therapy session
  - "It seems you're having difficulty. Would you like to:
    - Talk about your feelings?
    - Journal about this experience?
    - Take a mindfulness break?
    - Blame the patriarchy?"

- [ ] **Accessibility Features** (to an absurd degree):
  - Option to play with eyes closed (for sighted privilege awareness)
  - Seizure warning before every frame
  - Auto-play mode (game plays itself while you learn about social justice)
  - "Explain Like I'm a 5-Year-Old Whose Emotional Needs Must Be Centered" mode

### New Game Modes
- [ ] **Reparations Mode**: Give all points to oppressed buildings
- [ ] **Allyship Training**: Watch without playing, just observe and reflect
- [ ] **Cancelled Mode**: One mistake and game deletes itself
- [ ] **Tone Police Difficulty**: Every action judged on emotional impact, not results

### Audio
- [ ] **Affirmation Sounds**:
  - Seed drop: "You are valid!"
  - Building healed: "We're all healing together"
  - Level complete: "Your labor is appreciated, but remember this is lifelong work"
  - Game over: "Failure is just another word for learning opportunity"

- [ ] **Background Narration**:
  - Constant validation: "You're doing amazing, sweetie"
  - Gentle reminders: "Remember to check your privilege"
  - Educational moments: "Fun fact: This game engine was built on stolen code from BIPOC developers (probably)"

### Menus
- [ ] **Main Menu Options**:
  - Start Journey of Collective Healing
  - Review Our Diversity Statement
  - Read Our 47-Page Code of Conduct
  - Report Microaggression in Game Design
  - Donate to Causes (required before playing)
  - Submit Your Pronouns Again (They Might Have Changed)
  - Acknowledge This Game's Problematic Elements

- [ ] **Settings Menu**:
  - Trigger Warning Sensitivity (Low/Medium/Excessive/Paralyzing)
  - Guilt Intensity Slider
  - How Often to Remind Player of Systemic Oppression
  - Auto-Apologize Frequency
  - Virtue Signal Volume

### End Game
- [ ] **Victory Conditions**:
  - There is no victory, only ongoing work
  - "You've completed level 1, but the fight for justice never ends"
  - Credits roll acknowledging every marginalized group
  - Final screen: "Thanks for playing, but remember: being a good ally means doing better every day. See you tomorrow for your mandatory anti-bias training."

- [ ] **High Score Renamed**: "Allyship Progress Tracking (Not a Competition, We Don't Believe in Hierarchies)"

### Easter Eggs
- [ ] Konami Code replaced with "Acknowledge, Apologize, Amplify, Atone" sequence
- [ ] Secret level: "The Gender Spectrum" - infinite buildings representing infinite genders
- [ ] Hidden character: Bernie Sanders' mittens flying the plane
- [ ] Cheat code: Type "IBLAMECAPITALISM" for invincibility

### Performance Anxiety Features
- [ ] Game randomly pauses to ask: "Are you sure you're doing enough?"
- [ ] "You've been playing for 15 minutes. Shouldn't you be protesting instead?"
- [ ] Guilt-based achievements: "You enjoyed this game while others suffer"

### DLC Ideas
- [ ] "Intersectionality Pack" - adds more oppression categories
- [ ] "Therapy Session Expansion" - longer emotional support cutscenes
- [ ] "Cancel Culture Challenge Mode" - one mistake erases all progress
- [ ] "Performative Allyship Detector" - judges your authenticity

---

## Technical Implementation Notes

### Bomber-1982
- Keep simple, retro aesthetic
- Optimize for 60 FPS gameplay
- Minimal dependencies
- Classic arcade feel

### Bomber-2025
- May require trigger warning database
- Implement consent API for buildings
- Machine learning to detect microaggressions in player behavior
- Blockchain integration for virtue signal tracking (Web3 for social justice!)
- NFTs of each healed building (proceeds go to "charity" - mostly marketing)
- Always-online requirement (to push updates reminding players of their privilege)
- Telemetry tracking every action for "inclusion metrics"

---

## Platform Support

### Bomber-1982
- Linux (current)
- Windows
- macOS
- Android
- iOS
- Web (WASM)
- Raspberry Pi
- Actual VIC-20 (if possible)

### Bomber-2025
- Only on platforms owned by companies with perfect ESG scores (none exist)
- Requires signing DEI pledge before installation
- Must be connected to social media for automatic virtue signaling
- Blocks installation on computers that have ever run "problematic" software

---

## Release Strategy

### Bomber-1982
- Free and open source
- MIT License
- Available on itch.io, GitHub
- Community contributions welcome

### Bomber-2025
- $59.99 base game
- $29.99 per pronoun pack DLC
- $99.99 "Lifetime Allyship License"
- Battle pass with seasonal social justice causes
- Loot boxes containing random affirmations
- All profits go to "administration costs" of awareness campaigns
- Early Access: Pay to beta test while we figure out more things to be offended by
- Release contingent on approval from 47 different advocacy groups
- Mandatory review by sensitivity readers before each patch

---

**Note**: Bomber-2025 is a satirical over-the-top parody and should not be taken seriously. It's a humorous exaggeration of performative progressivism in modern gaming. The actual Bomber-1982 remains a fun, simple retro game for everyone to enjoy.
