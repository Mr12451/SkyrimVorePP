#pragma once

namespace Vore
{
	class Sounds
	{
	public:
		static inline bool AllowSounds{ false };

		static inline RE::BGSSoundDescriptorForm* Bones_F{ nullptr }; //unused; sounds too low quality, poop is better
		static inline RE::BGSSoundDescriptorForm* Bones_M{ nullptr }; //unused; missing sounds
		static inline RE::BGSSoundDescriptorForm* Burp{ nullptr };
		static inline RE::BGSSoundDescriptorForm* CockVore{ nullptr }; //unused; add sounds for alt vore types later
		static inline RE::BGSSoundDescriptorForm* FootstepSlosh{ nullptr };
		static inline RE::BGSSoundDescriptorForm* Gurgle{ nullptr };
		static inline RE::BGSSoundDescriptorForm* Poop{ nullptr }; //+
		static inline RE::BGSSoundDescriptorForm* Poop_F{ nullptr }; //+
		static inline RE::BGSSoundDescriptorForm* Poop_M{ nullptr }; //+
		static inline RE::BGSSoundDescriptorForm* Screamless_F{ nullptr }; //+
		static inline RE::BGSSoundDescriptorForm* Screamless_M{ nullptr }; //+
		static inline RE::BGSSoundDescriptorForm* Scream_A{ nullptr }; //+
		static inline RE::BGSSoundDescriptorForm* Scream_C{ nullptr }; //+
		static inline RE::BGSSoundDescriptorForm* Scream_F{ nullptr }; //+
		static inline RE::BGSSoundDescriptorForm* Scream_M{ nullptr }; //+
		static inline RE::BGSSoundDescriptorForm* Scream_U{ nullptr }; //+
		static inline RE::BGSSoundDescriptorForm* Stomach{ nullptr };
		static inline RE::BGSSoundDescriptorForm* Struggling_CF{ nullptr };
		static inline RE::BGSSoundDescriptorForm* Struggling_CM{ nullptr };
		static inline RE::BGSSoundDescriptorForm* Struggling_FF{ nullptr };
		static inline RE::BGSSoundDescriptorForm* Struggling_FM{ nullptr };
		static inline RE::BGSSoundDescriptorForm* Struggling_MF{ nullptr };
		static inline RE::BGSSoundDescriptorForm* Struggling_MM{ nullptr };
		static inline RE::BGSSoundDescriptorForm* Swallow{ nullptr }; //+
		static inline RE::BGSSoundDescriptorForm* Vomit{ nullptr }; //+

		static void InitSounds();
	};
}