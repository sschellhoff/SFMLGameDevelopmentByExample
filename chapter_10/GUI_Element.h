#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <string>
#include <unordered_map>

#include "GUI_Style.h"
#include <sstream>

enum class GUI_ElementType{ None = -1, Window, Label, Button, Scrollbar, Textfield };

using ElementStyles = std::unordered_map<GUI_ElementState, GUI_Style>;

class GUI_Interface;

class GUI_Element{
	friend class GUI_Interface;
public:
	GUI_Element(const std::string& l_name, const GUI_ElementType& l_type, GUI_Interface* l_owner);
	virtual ~GUI_Element();

	// Event methods.
	virtual void ReadIn(std::stringstream& l_stream) = 0;
	virtual void OnClick(const sf::Vector2f& l_mousePos) = 0;
	virtual void OnRelease() = 0;
	virtual void OnHover(const sf::Vector2f& l_mousePos) = 0;
	virtual void OnLeave() = 0;
	virtual void Update(float l_dT) = 0;
	virtual void Draw(sf::RenderTarget* l_target) = 0;

	// Non pure-virtual methods.
	virtual void UpdateStyle(const GUI_ElementState& l_state, const GUI_Style& l_style);
	virtual void ApplyStyle();

	GUI_ElementType GetType();

	const std::string& GetName() const;
	void SetName(const std::string& l_name);
	const sf::Vector2f& GetPosition() const;
	void SetPosition(const sf::Vector2f& l_pos);
	const sf::Vector2f& GetSize() const;
	GUI_ElementState GetState() const;
	void SetState(const GUI_ElementState& l_state);
	void SetRedraw(const bool& l_redraw);
	bool NeedsRedraw() const;
	void SetOwner(GUI_Interface* l_owner);
	GUI_Interface* GetOwner() const;
	bool HasOwner() const;
	bool IsActive() const;
	void SetActive(const bool& l_active);
	bool IsInside(const sf::Vector2f& l_point) const;
	sf::Vector2f GetGlobalPosition() const;
	bool IsControl() const;
	std::string GetText() const;
	void SetText(const std::string& l_text);

	friend std::stringstream& operator >>(
		std::stringstream& l_stream, GUI_Element& b)
	{
		b.ReadIn(l_stream);
		return l_stream;
	}
protected:
	void ApplyTextStyle();
	void ApplyBgStyle();
	void ApplyGlyphStyle();

	void RequireTexture(const std::string& l_name);
	void RequireFont(const std::string& l_name);
	void ReleaseTexture(const std::string& l_name);
	void ReleaseFont(const std::string& l_name);
	void ReleaseResources();
	std::string m_name;
	sf::Vector2f m_position;
	ElementStyles m_style; // Style of drawables.
	GUI_Visual m_visual; // Drawable bits.
	GUI_ElementType m_type;
	GUI_ElementState m_state;
	GUI_Interface* m_owner;

	bool m_needsRedraw;
	bool m_active;
	bool m_isControl;
};