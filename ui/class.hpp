#pragma once

/// User interface namespace.
namespace UI {
	/// Style dimension structure.
	struct Dim {
		int   off; /// Base offset.
		float par; /// Parent offset.
		float box; /// Size offset.

		/// Empty constructor.
		Dim (): off(0), par(0.0f), box(0.0f) {};

		/// Default constructor.
		/// @param off Base offset.
		/// @param par Parent offset.
		/// @param box Size offset.
		Dim (int off, float par, float box): off(off), par(par), box(box) {};

		/// Creates a dimension from base offset.
		static Dim pix(int off) { return Dim(off, 0.0f, 0.0f); };
		/// Creates a dimension from base offset.
		static Dim parent(float off) { return Dim(0, off, 0.0f); };
		/// Creates a dimension from base offset.
		static Dim size(float off) { return Dim(0, 0.0f, off); };

		/// Adds 2 dimensions.
		/// @param oth Other dimension.
		Dim operator+(const Dim& dim) const { return Dim(off + dim.off, par + dim.par, box + dim.box); };
		/// Subtracts 2 dimensions.
		/// @param oth Other dimension.
		Dim operator-(const Dim& dim) const { return Dim(off - dim.off, par - dim.par, box - dim.box); };
		/// Negates a dimension.
		Dim operator-() const { return Dim(-off, -par, -box); };
		/// Scales a dimension.
		/// @param scale Scaling factor.
		Dim operator*(int scale) const { return Dim(off * scale, par * scale, box * scale); };
		/// Scales a dimension.
		/// @param scale Scaling factor.
		Dim operator*(float scale) const { return Dim(off * scale, par * scale, box * scale); };

		/// Returns dimension value.
		/// @param par Parent size.
		/// @param box Element size.
		int get(int par, int box) const { return off + par * this->par + box * this->box; };

		/// Linearly interpolates between 2 dimensions.
		/// @param a Starting dimension.
		/// @param b Ending dimension.
		/// @param t Interpolation coefficient.
		/// @return Interpolated dimension.
		static Dim lerp(Dim a, Dim b, float t) {
			return Dim(lerpi(a.off, b.off, t), lerpf(a.par, b.par, t), lerpf(a.box, b.box, t));
		};
	};

	/// 2D dimension vector.
	struct Dim2 {
		Dim x; /// X axis dimension.
		Dim y; /// Y axis dimension.

		/// Empty constructor.
		Dim2 () {};
		/// Default constructor.
		/// @param x X axis dimension.
		/// @param y Y axis dimension.
		Dim2 (Dim x, Dim y): x(x), y(y) {};

		/// Adds 2 vectors.
		/// @param oth Other vector.
		Dim2 operator+(const Dim2& vec) const { return Dim2(x + vec.x, y + vec.y); };
		/// Subtracts 2 vectors.
		/// @param oth Other vector.
		Dim2 operator-(const Dim2& vec) const { return Dim2(x - vec.x, y - vec.y); };
		/// Scales a dimension vector.
		/// @param scale Scaling factor.
		Dim2 operator*(int scale) const { return Dim2(x * scale, y * scale); };
		/// Scales a dimension vector.
		/// @param scale Scaling factor.
		Dim2 operator*(float scale) const { return Dim2(x * scale, y * scale); };

		/// Creates a dimension vector from a vector.
		/// @param vec Integer vector.
		static Dim2 fromVec(sf::Vector2i vec) {
			return Dim2(
				Dim::pix(vec.x),
				Dim::pix(vec.y)
			);
		};

		/// Returns vector value.
		/// @param par Parent size.
		/// @param box Element size.
		sf::Vector2i get(sf::Vector2i par, sf::Vector2i box) const {
			return sf::Vector2i(x.get(par.x, box.x), y.get(par.y, box.y));
		};

		/// Linearly interpolates between 2 dimension vectors.
		/// @param a Starting dimension vector.
		/// @param b Ending dimension vector.
		/// @param t Interpolation coefficient.
		/// @return Interpolated dimension vector.
		static Dim2 lerp(Dim2 a, Dim2 b, float t) {
			return Dim2(Dim::lerp(a.x, b.x, t), Dim::lerp(a.y, b.y, t));
		};
	};

	/// 2D dimension bounding box.
	struct DimBox {
		Dim2  pos; /// Box position.
		Dim2 size; /// Box size.

		/// Empty constructor.
		DimBox () {};
		/// Vector constructor.
		/// @param pos Box position.
		/// @param size Box size.
		DimBox (Dim2 pos, Dim2 size): pos(pos), size(size) {};
		/// Scalar constructor.
		/// @param x X position.
		/// @param y Y position.
		/// @param w Width.
		/// @param h Height.
		DimBox (Dim x, Dim y, Dim w, Dim h): pos(Dim2(x, y)), size(Dim2(w, h)) {};

		/// Creates a dimension box from a rectangle.
		/// @param rect Integer rectangle.
		static DimBox fromRect(sf::IntRect rect) {
			return DimBox(
				Dim::pix(rect.left),
				Dim::pix(rect.top),
				Dim::pix(rect.width),
				Dim::pix(rect.height)
			);
		};

		/// Get bounding box rectangle.
		/// @param parent Parent rectangle.
		sf::IntRect get(sf::IntRect parent) const {
			sf::Vector2i bounds = size.get(sf::Vector2i(parent.width, parent.height), sf::Vector2i());
			sf::Vector2i offset = pos.get(sf::Vector2i(parent.width, parent.height), bounds);
			return sf::IntRect(offset + sf::Vector2i(parent.left, parent.top), bounds);
		};
	};

	/// UI element event callback data.
	struct EventData {
		sf::Vector2i      pos; /// Event position.
		sf::Keyboard::Key key; /// Event key.
	};

	/// Base UI class.
	class Element {
		public:
		/// Event handler function.
		/// @param self Element reference.
		/// @param data Event data.
		/// @return Whether the event was processed.
		using Handler = std::function<bool (Element&, const EventData&)>;

		/// Element update function.
		/// @param self Element reference.
		/// @param delta Time elapsed since last frame.
		using Update = std::function<void (Element&, float)>;

		/// Element event type.
		enum Event {
			MouseDown = 0,
			MouseUp,
			MouseHover,
			MouseEnter,
			MouseLeave,
			KeyDown,
			KeyUp,
			Count
		};

		/// Virtual destructor.
		virtual ~Element() {};

		protected:
		/// Draws the element itself.
		/// @param self Bounding box.
		virtual void drawSelf(sf::IntRect self) const {};

		/// Draws the child elements.
		/// @param self Bounding box.
		virtual void drawChildren(sf::IntRect self) const {
			for (const std::unique_ptr<Element>& elem : children)
				elem->draw(self);
		};

		/// Element event handler list.
		std::vector<Handler> handlers[Event::Count];
		/// Previous hover value.
		bool p_hover = false;

		public:
		/// Element bounding box.
		DimBox box;
		/// Element children.
		std::vector<std::unique_ptr<Element>> children;
		/// Whether the element is active.
		bool active = true;
		/// Whether the element ignores events.
		bool ignore = false;
		/// Whether the element is being hovered on.
		bool hover = false;
		/// Update callback.
		std::vector<Update> updcalls;

		/// Adds a child element.
		/// @param elem Heap-allocated element.
		void add(Element* elem) {
			children.push_back(std::unique_ptr<Element>(elem));
		};

		/// Removes a child element by its pointer.
		/// @param elem Element pointer.
		/// @note If an element is not found, no action will be taken.
		void remove(Element* elem) {
			for (size_t i = 0; i < children.size(); i++) {
				if (children[i].get() == elem) {
					children.erase(children.begin() + i);
					return;
				};
			};
		};

		/// Replaces a child element.
		/// @param old Pointer to previous element.
		/// @param rep New heap-allocated element.
		/// @note If an old element is not found, replacement will be just added.
		void swap(Element* old, Element* rep) {
			// try to replace old object
			for (size_t i = 0; i < children.size(); i++) {
				if (children[i].get() == old) {
					children[i] = std::unique_ptr<Element>(rep);
					return;
				};
			};

			// insert if no replacement found
			add(rep);
		};

		/// Adds an element event callback.
		/// @param type Event type.
		/// @param handler Handler callback.
		void eventCall(Event type, Handler handler) {
			handlers[type].push_back(handler);
		};

		/// Adds an element update callback.
		/// @param handler Update callback.
		void updateCall(Update handler) {
			updcalls.push_back(handler);
		};

		/// Updates hover field for the element.
		/// @param par Parent bounding box.
		/// @param pos Mouse position.
		void mouse(sf::IntRect par, sf::Vector2i pos) {
			// store previous hover value
			p_hover = hover;

			// check if element is inactive
			if (!active || ignore) { hover = false; return; };

			// calculate bounding box
			sf::IntRect self = box.get(par);

			// check for child overlap
			for (const std::unique_ptr<Element>& elem : children)
				elem->mouse(self, pos);

			// check for element overlap
			hover = self.contains(pos);

			// trigger hover related events
			if (hover && !p_hover) for (const Handler& handler : handlers[MouseEnter]) handler(*this, { pos, sf::Keyboard::Unknown });
			if (hover            ) for (const Handler& handler : handlers[MouseHover]) handler(*this, { pos, sf::Keyboard::Unknown });
			if (!hover && p_hover) for (const Handler& handler : handlers[MouseLeave]) handler(*this, { pos, sf::Keyboard::Unknown });
		};

		/// Processes an event on the element.
		/// @param par Parent bounding box.
		/// @param evt Event type.
		/// @param data Event data.
		/// @return Whether the event was captured.
		bool event(sf::IntRect par, Event evt, const EventData& data) {
			// check if element is inactive
			if (!active || ignore) return false;

			// calculate bounding box
			sf::IntRect self = box.get(par);

			// check for child overlap
			for (const std::unique_ptr<Element>& elem : children)
				if (elem->event(self, evt, data))
					return true;

			// check for element overlap for mouse events
			if ((evt == MouseDown || evt == MouseUp) && !self.contains(data.pos))
				return false;

			// handle the event
			return forceEvent(evt, data);
		};

		/// Forces an event on the element.
		/// @param evt Event type.
		/// @param data Event data.
		/// @return Whether the event was captured.
		/// @note This event does not propagate on children.
		bool forceEvent(Event evt, const EventData& data) {
			for (const Handler& handler : handlers[evt])
				if (handler(*this, data)) return true;
			return false;
		};

		/// Updates the element and its children.
		/// @param delta Time elapsed since last frame.
		void update(float delta) {
			// check if element is inactive
			if (!active) return;

			// update children
			for (const std::unique_ptr<Element>& elem : children)
				elem->update(delta);

			// update element
			for (const Update& upd : updcalls)
				upd(*this, delta);
		};

		/// Draws the element and its children.
		/// @param parent Parent bounding box.
		void draw(sf::IntRect parent) const {
			// check if element is inactive
			if (!active) return;

			// calculate bounding box
			sf::IntRect self = box.get(parent);

			// draw element and children
			drawSelf(self);
			drawChildren(self);
		};

		/// Attaches the element below the target.
		/// @param target Target element.
		/// @param padding Space between elements.
		void attachBelow(Element* target, Dim padding) {
			updateCall([=](Element& elem, float) {
				elem.box.pos.y = target->box.pos.y + target->box.size.y + padding;
			});
		};
	};

	/// Void handler function.
	bool voidCallback(Element&, sf::Vector2i) { return true; };
};

/// Pixel dimension literal.
UI::Dim operator ""px(unsigned long long val) { return UI::Dim(val, 0.0f, 0.0f); };
/// Parent size dimension literal.
UI::Dim operator ""ps(long double val) { return UI::Dim(0, (float)val, 0.0f); };
/// Parent size dimension literal.
UI::Dim operator ""ps(unsigned long long val) { return UI::Dim(0, (float)val, 0.0f); };
/// Element size dimension literal.
UI::Dim operator ""ws(long double val) { return UI::Dim(0, 0.0f, (float)val); };
/// Element size dimension literal.
UI::Dim operator ""ws(unsigned long long val) { return UI::Dim(0, 0.0f, (float)val); };