# Everything But Titans

Developed in Unreal Engine 5.6

Everything But Titans is a fast-paced, vertical arena shooter prototype inspired by the high-mobility "jetpack" eras of classic FPS titles. Designed from the ground up for both single-player and fully replicated multiplayer, this project serves as a deep dive into advanced Unreal Engine frameworks, low-level C++ architecture, and real-world networking.

🛠️ Tech Stack & Frameworks
-Engine: Unreal Engine 5.6
-Languages: C++, Blueprints
-Core Frameworks: Gameplay Ability System (GAS), State Tree AI
-Networking: Steam SDK (Steamworks), Client-Server Replication, RPCs, Client-Side Prediction

⚙️ Technical Architecture & Core Systems
The primary goal of this project was to push the boundaries of Unreal's modern toolset through extensive research and custom engineering, focusing on highly scalable mechanics and robust server-client architecture.
-Advanced GAS via Custom C++: Rather than relying on standard Blueprint abilities, the core Gameplay Ability System (GAS) architecture is engineered in C++. By building custom UGameplayAbility functions and intricate Gameplay Cues, the framework is highly scalable. Complex mechanics—like omni-directional jetpacks and momentum-based dashes—are transformed into modular, data-driven assets that can be endlessly combined and iterated upon without bloating the codebase.

-Steam SDK & Real-World Replication: Moving beyond idealized LAN environments, the project integrates the Steam SDK to handle session creation, matchmaking, and live player connections. It utilizes a server-authoritative architecture to ensure fairness in a high-mobility environment. By leveraging GAS’s built-in client-side prediction, high-speed aerial combat feels instantly responsive for the client while remaining perfectly synchronized across the network.

-AI Research: State Trees vs. Behavior Trees: Enemy bots utilize Unreal’s newer State Tree system rather than traditional Behavior Trees. Research and testing determined that while Behavior Trees excel at complex, long-term hierarchical tasks, State Trees provide a much tighter, highly optimized state-machine structure. This makes them the superior choice for the fast-paced, reaction-heavy logic of an arena shooter, allowing bots to efficiently evaluate transitions between aerial evasion, ground traversal, and combat with minimal performance overhead.

🎮 Development Focus: The "Feel" of Combat
Creating a compelling "jetpack shooter" requires incredibly tight controls. By centralizing the movement and combat logic within the custom GAS framework, variables like gravity scaling, thrust velocity, and air-strafing parameters can be rapidly tweaked. This data-driven approach allows for real-time iteration on the "feel" of the game, ensuring the aerial combat is fluid, momentum-driven, and highly rewarding.
