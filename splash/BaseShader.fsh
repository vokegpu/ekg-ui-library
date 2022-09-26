#version 450 core

layout (location = 0) out vec4 OutColor;

in vec2 TextureUV;
in vec4 ShapeRect;

uniform int LineThickness;
uniform int ViewportHeight;
uniform bool ActiveTexture;
uniform Sampler2D ActiveTextureSlot;
uniform vec4 Color;

void main() {
	OutColor = Color;

	if (LineThickness > 0) {
		vec2 FragPos = vec2(gl_FragCoord.x, ViewportHeight - gl_FragCoord.y);
		vec4 OutlineRect = vec4(ShapeRect.x + LineThickness, ShapeRect.y + LineThickness, ShapeRect.z - LineThickness * 2, ShapeRect.w - LineThickness * 2);
		
		bool Collide = FragPos.x > OutlineRect.x && FragPos.x < OutlineRect.x + OutlineRect.z && FragPos.y > OutlineRect.y && FragPos.y < OutlineRect.y + OutlineRect.w;

		if (Collide) {
			discard;
		}
	}

	if (ActiveTexture) {
		OutColor = texture(ActiveTextureSlot, TextureUV);
		OutColor = vec4(OutColor.xyz - ((1.0f - Color.xyz) - 1.0f), OutColor.w - (1.0f - Color.w));
	}
}