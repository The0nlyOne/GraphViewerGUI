#pragma once

#include<stack>
#include "Command.hpp"

namespace Model
{

	typedef std::stack<std::shared_ptr<Command>> CommandsStack;
	class Invoker
	{
	public:
		// Method to instanciate the only instance of the invoker
		static Invoker* getInvoker()
		{
			static Invoker onlyInstance;
			return &onlyInstance;
		}

		void executeCommand(std::shared_ptr<Command> command)
		{
			redoStack = CommandsStack(); // reinitialize the redoStack
			command->execute();
			cancelStack.push(command);
		}

		void cancel()
		{
			if (cancelStack.size() <= 0)
				return;
			cancelStack.top()->cancel();		// cancel the most recent command
			redoStack.push(cancelStack.top());	// add the canceled command in redoStack
			cancelStack.pop();					// remove the canceled command from the cancelStack
		}

		void redo()
		{
			if (redoStack.size() <= 0)
				return;
			redoStack.top()->redo();			// redo the most recent command
			cancelStack.push(redoStack.top());	// add that command to the cancelStack
			redoStack.pop();					// remove that command from the redoStack
		}

	private:
		Invoker() = default;
		CommandsStack cancelStack;
		CommandsStack redoStack;
	};

}