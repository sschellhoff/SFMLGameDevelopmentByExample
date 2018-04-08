#pragma once
#include "GUI_Element.h"
#include <unordered_map>

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>

using Elements = std::unordered_map<std::string,GUI_Element*>;

class GUI_Manager;

class GUI_Interface : public GUI_Element{
	friend class GUI_Element;
	friend class GUI_Manager;
public:
	GUI_Interface(const std::string& l_name, GUI_Manager* l_guiManager);
	~GUI_Interface();

	void SetPosition(const sf::Vector2f& l_pos);

	bool AddElement(const GUI_ElementType& l_type, const std::string& l_name);
	GUI_Element* GetElement(const std::string& l_name) const;
	bool RemoveElement(const std::string& l_name);

	bool HasParent() const;
	GUI_Manager* GetManager() const;

	bool IsInside(const sf::Vector2f& l_point) const;

	void Focus();
	void Defocus();
	bool IsFocused();

	void ReadIn(std::stringstream& l_stream);
	void OnClick(const sf::Vector2f& l_mousePos);
	void OnRelease();
	void OnHover(const sf::Vector2f& l_mousePos);
	void OnLeave();
	void OnTextEntered(const char& l_char);

	bool IsBeingMoved() const;
	bool IsMovable() const;
	void BeginMoving();
	void StopMoving();

	void ApplyStyle();

	const sf::Vector2f& GetPadding() const;
	void SetPadding(const sf::Vector2f& l_padding);
	sf::Vector2f GetGlobalPosition() const;
	const sf::Vector2f& GetContentSize() const;

	void Redraw();
	bool NeedsContentRedraw() const;
	void RedrawContent();
	bool NeedsControlRedraw() const;
	void RedrawControls();

	void ToggleTitleBar();

	void Update(float l_dT);
	void Draw(sf::RenderTarget* l_target);

	void UpdateScrollHorizontal(unsigned int l_percent);
	void UpdateScrollVertical(unsigned int l_percent);
private:
	void DefocusTextfields();
	Elements m_elements;
	sf::Vector2f m_elementPadding;

	GUI_Interface* m_parent;
	GUI_Manager* m_guiManager;

	sf::RenderTexture* m_backdropTexture;
	sf::Sprite m_backdrop;

	// Movement.
	sf::RectangleShape m_titleBar;
	sf::Vector2f m_moveMouseLast;
	bool m_showTitleBar;
	bool m_movable;
	bool m_beingMoved;
	bool m_focused;

	// Variable size.
	void AdjustContentSize(const GUI_Element* l_reference = nullptr);
	void SetContentSize(const sf::Vector2f& l_vec);
	sf::RenderTexture* m_contentTexture;
	sf::Sprite m_content;
	sf::Vector2f m_contentSize;
	int m_scrollHorizontal;
	int m_scrollVertical;
	bool m_contentRedraw;

	// Control layer.
	sf::RenderTexture* m_controlTexture;
	sf::Sprite m_control;
	bool m_controlRedraw;
};