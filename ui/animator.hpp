#pragma once

/// User interface namespace.
namespace UI {
	/// Base animator class.
	class Animator {
		protected:
		float  time = 0.0f;            /// Animation time.
		float  dura = 0.0f;            /// Animation duration.
		Easing ease = Easings::linear; /// Animation easing.
		bool   prog = false;           /// Whether the animation is in progress.

		/// Ticks animation.
		/// @param t Animation interpolation progress.
		virtual void tick(float t) {};

		/// Takes action after the animation.
		virtual void end() {};

		public:
		/// Stops the animation.
		void stop() { prog = false; };

		/// Ticks animation.
		/// @param delta Time elapsed since last frame.
		void update(float delta) {
			// check for no progress
			if (!prog) return;

			// update timer
			time += delta;
			if (time >= dura) {
				time = dura;
				prog = false;
				tick(1.0f);
				end();
			}

			// tick animation
			else tick(ease(time / dura));
		};

		/// Checks if animation is active.
		bool active() const { return prog; };

		/// Checks how much animation time is left.
		float left() const { return prog ? dura - time : 0.0f; };

		/// Sets animator easing function.
		/// @return Self-reference.
		Animator& setEasing(Easing easing) {
			ease = easing;
			return *this;
		};
	};

	/// Animator class with no target.
	class AnimVoid : public Animator {
		protected:
		/// Animation finish callback.
		std::function<void (AnimVoid&)> endcall;

		/// Ends animation.
		void end() override { if (endcall) endcall(*this); };

		public:
		/// Creates a waiting animation.
		/// @param time Waiting time.
		/// @param callback Animation ending callback.
		AnimVoid (float time, std::function<void (AnimVoid&)> callback) {
			endcall = callback;
			this->time = 0.0f;
			this->dura = time;
			this->prog = true;
		};
	};

	/// Linear animator class.
	/// @tparam T Animated type.
	/// @tparam L Animated type linear interpolator function.
	template <typename T, T (L)(T, T, float)> class AnimLinear : public Animator {
		static_assert(std::is_copy_constructible<T>());
		static_assert(std::is_copy_assignable<T>());

		protected:
		T  a_beg; /// Animation starting value.
		T  a_end; /// Animation ending value.
		T* a_tar; /// Animation target.

		/// Animation finish callback.
		std::function<void (AnimLinear&)> endcall;

		/// Ticks animation.
		/// @param t Animation interpolation progress.
		void tick(float t) override { *a_tar = L(a_beg, a_end, t); };
		/// Ends animation.
		void end() override { if (endcall) endcall(*this); };

		public:
		/// Binds a callback to animation ending.
		/// @param call Callback.
		void finish(std::function<void (AnimLinear&)> call) {
			endcall = call;
		};

		/// Returns animation target.
		T& target() { return *a_tar; };

		/// Starts an animation.
		/// @param target Animation target.
		/// @param start Starting value.
		/// @param end Ending value.
		/// @param time Animation duration.
		void start(T* target, const T& start, const T& end, float time) {
			a_beg = start;
			a_end = end;
			a_tar = target;
			this->time = 0.0f;
			this->dura = time;
			this->prog = true;
		};

		/// Starts an animation.
		/// @param target Animation target.
		/// @param end Ending value.
		/// @param time Animation duration.
		void start(T* target, const T& end, float time) { start(target, *target, end, time); };
	};

	/// Integer animator.
	using AnimInt = AnimLinear<int, lerpi>;
	/// Float animator.
	using AnimFloat = AnimLinear<float, lerpf>;
	/// Dimension animator.
	using AnimDim = AnimLinear<Dim, Dim::lerp>;
	/// Dimension vector animator.
	using AnimDim2 = AnimLinear<Dim2, Dim2::lerp>;
	/// Color vector animator.
	using AnimColor = AnimLinear<glm::vec3, lerpv>;
};