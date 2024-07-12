#pragma once

#include "core/font_component.hxx"
#include "core/object.hxx"
#include "core/sprite_component.hxx"

namespace gl::app
{
	class subsubscriber_ghost : public object
	{
	public:
		subsubscriber_ghost(const std::string& subTitle, const std::string& subId);
		~subsubscriber_ghost();

		void init() override;
		void update(double delta) override;
		void draw(SDL_Renderer* renderer) override;

		const std::string& getChannelId() const
		{
			return mSubId;
		};

	private:
		sprite_component* mSpriteComponent;
		font_component* mFontComponent;

		std::string mSubTitle;
		std::string mSubId;

		double mX{}, mY{};		  // current screen pos
		int32_t mDstX{}, mDstY{}; // dest screen pos
	};
} // namespace gl::app