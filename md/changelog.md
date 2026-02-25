### 1.5.2 (2/25/2026)

- Migrated to **Geode v5 (2.2081)**
- Added **number inputs** to all HSV widgets
- Fixed **Named Editor Group buttons** being removed in some triggers
- Fixed **Add Random Groups popup** having an enter animation

### 1.5.1 (1/14/2026)

- Added **ranges and series** to Advanced Filter
- Fixed **crash when using multi-edit** with HSV
- Fixed **HSV sometimes showing as NaN**
- Fixed multi-edit not working in **shake, animate, and follow player Y triggers**
- Fixed next free button **overlapping buttons** in edit area trigger
- Fixed next free button **not showing** in some inputs
- Fixed labels being **too long** in follow trigger

### 1.5.0 (1/8/2026)

- Added **select all button** to Advanced Filter
- Added **next free collision block, item, timer, control, and material ID** buttons
- Added **next free buttons** to trigger menus which previously lacked them, such as alpha and collision
- Fixed **Saw Rotation Preview** displaying incorrectly on non-16:9 aspect ratios
- Fixed **Hide UI button** not working on desktop
- Fixed **softlock with multi-edit** in advanced follow
- Fixed multi-edit not setting **several inputs** in advanced follow
- Fixed multi-edit not setting **copy color ID inputs**
- Fixed quick paste button **overriding the last used preset**
- Fixed **objects being unselectable** after pasting position state
- Fixed **visual errors** after pasting editor layer state
- Fixed **object editor layer updating** after pasting object ID state
- Fixed **incorrect label shown** when using the opacity slider with Hide UI enabled in alpha trigger

Additionally, multi-edit was rewritten from scratch yet again because the first rewrite wasn't very good whoops.

### 1.4.0 (6/29/2025)

- Added **Saw Rotation Preview**
- Added fix for vanilla bug where touch, count, instant count, random, camera zoom, camera offset, camera edge, and on death trigger menus **only support one trigger**
- Added setting to **change the hide trigger UI key**, since AZERTY keyboard layouts use shift for number keys
- Bumped Geode version to support **JIT-less iOS**
- Fixed multi-edit HSV **incorrectly showing mixed state** when there is none
- Fixed crash when using BetterEdit's **Paste State keybind**

### 1.3.3 (5/4/2025)

- Fixed Android crash when **using a slider in particle triggers** <c-9CA0AF>(thank you again [hiimjasmine00](user:7466002)!)</c>
- Fixed overlapping inputs causing **multiple multi-edit popups** to appear

### 1.3.2 (5/3/2025)

- Added button to **quickly paste default state** in the Advanced Paste State menu
- Added setting to **disable Advanced Paste State**
- Fixed multi-edit mode being **completely broken** <c-9CA0AF>(my bad yall)</c>
- Fixed **guides not appearing** when using a slider in particle triggers

### 1.3.1 (5/1/2025)

- Added **iOS support**
- Added Mac support for **right clicking on inputs** to open multi-edit mode <c-9CA0AF>(thank you [hiimjasmine00](user:7466002)!)</c>
- Fixed crash when **opening Advanced Paste State menu**
- Fixed crash with multi-edit mode when there are **no valid objects**

### 1.3.0 (4/24/2025)

- Added **Advanced Paste State**
- Added multi-edit mode to **HSV sliders**
- Added hide UI and multi-edit support to **particle triggers, collision triggers, and collision blocks**
- Fixed multi-edit not setting **follow mod X and Y**
- Fixed **minor visual bugs** with next free group buttons
- Fixed mixed buttons **not hiding properly** in move triggers

Additionally, multi-edit mode and hide UI were rewritten from scratch. You won't notice anything as a user, but under the hood they're now much more stable and reusable!

### 1.2.1 (2/16/2025)

- Added ability to **right click on an input** to enter multi-edit mode (Windows only)
- Added **next free button** to target & center group controls
- Fixed crashes caused by entering an **invalid string** into a number input

### 1.2.0 (1/31/2025)

- Added **Improved Select Filter**
- Fixed **unclickable button** in Add Random Groups popup
- Fixed crash when **enabling multi-edit mode** after hiding the UI
- Fixed **sliders being low opacity** after hiding the UI

### v1.1.1 (1/2/2025)

- Fixed duration controls being **limited to whole numbers** in Improved Edit Triggers Popup

### 1.1.0 (1/1/2025)

- Migrated to **Geode v4 (2.2074)**
- Added **Multi-Edit Mode** to triggers
- Added **Improved Edit Triggers Popup**
- Added **Hide UI** button to triggers
- Removed **10 group limit** in Add Random Groups popup

### 1.0.0 (3/30/2024)

- Added **Add Random Groups**
