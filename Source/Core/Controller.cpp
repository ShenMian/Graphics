﻿// Copyright 2021 ShenMian
// License(Apache-2.0)

#include "Controller.h"
#include "Camera.h"
#include "Input.h"
#include "Gamepad.h"
#include <algorithm>

Controller::Controller(Camera& cam)
	: camera(&cam)
{
}

void Controller::update(float dt)
{
	processMouse(dt);
	processKeyboard(dt);
	processGamepad(dt);

	if(smoothness)
		camera->setPosition(Vector3f::lerp(camera->getPosition(), target, dt * smoothness));
	else
		camera->setPosition(target);
}

void Controller::moveFront(float step)
{
	target += camera->getFront() * step;
}

void Controller::moveRight(float step)
{
	target += camera->getRight() * step;;
}

void Controller::moveUp(float step)
{
	target += -Vector3f::unit_y * step;;
}

void Controller::turnRight(float step)
{
	auto rot = camera->getRotation();
	rot.y += step;
	camera->setRotation(rot);
}

void Controller::lookUp(float step)
{
	auto rot = camera->getRotation();
	rot.x += step;
	rot.x = std::clamp(rot.x, -89.f, 89.f);
	camera->setRotation(rot);
}

void Controller::setSmoothness(float v)
{
	smoothness = v;
}

void Controller::setCamera(Camera& cam)
{
	camera = &cam;
	target = camera->getPosition();
}

void Controller::setGamepad(Gamepad& Gamepad)
{
	gamepad = &Gamepad;
}

void Controller::processKeyboard(float dt)
{
	float step = speed * dt;
	if(Input::isPressed(Key::LeftShift))
		step *= 3;

	if(Input::isPressed(Key::W))
		moveFront(step);
	if(Input::isPressed(Key::S))
		moveFront(-step);
	if(Input::isPressed(Key::A))
		moveRight(-step);
	if(Input::isPressed(Key::D))
		moveRight(step);
	if(Input::isPressed(Key::E))
		moveUp(step);
	if(Input::isPressed(Key::Q))
		moveUp(-step);
}

void Controller::processMouse(float dt)
{
	const Vector2f sensitivity = Vector2f::unit * 8.f * dt;

	const auto position = Input::getMousePosition();
	static Vector2f lastPos = position;
	const auto pos = static_cast<Vector2f>(position);
	Vector2f offset = pos - lastPos;
	lastPos = pos;
	offset.x *= sensitivity.x;
	offset.y *= sensitivity.y;

	lookUp(-offset.y);
	turnRight(offset.x);
}

void Controller::processGamepad(float dt)
{
	if(!gamepad->isConnected())
		return;

	gamepad->update();

	float step = speed * dt * 1.2f;
	if(gamepad->get(Gamepad::Button::LeftThumb))
		step *= 3;

	const Vector2f sensitivity = Vector2f::unit * 200.f * dt;

	const auto leftThumb = gamepad->get(Gamepad::Thumb::left);
	const auto rightThumb = gamepad->get(Gamepad::Thumb::right);
	const auto rightTrigger = gamepad->get(Gamepad::Trigger::right);
	const auto leftTrigger = gamepad->get(Gamepad::Trigger::left);
	moveFront(-leftThumb.y * step);
	moveRight(leftThumb.x * step);
	moveUp(rightTrigger + (-leftTrigger));
	lookUp(-rightThumb.y * sensitivity.x);
	turnRight(rightThumb.x * sensitivity.y);
}
