/*
 * Copyright (c) Thomas Nägele and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

package nl.ru.sws.cohla.generator.openrti.models;

public class ClassArgument {
	
	private final String name, helpText;
	private String defaultValue;
	private boolean asString = true, noCmdArg = false;
	
	public ClassArgument(String name, String helpText) {
		this.name = name;
		this.helpText = helpText;
	}
	
	public ClassArgument(String name, String helpText, String defaultValue) {
		this(name, helpText, defaultValue, true);
	}
	
	public ClassArgument(String name, String helpText, String defaultValue, boolean asString) {
		this(name, helpText, defaultValue, asString, false);
	}
	
	public ClassArgument(String name, String helpText, String defaultValue, boolean asString, boolean noCmdArg) {
		this.name = name;
		this.helpText = helpText;
		this.defaultValue = defaultValue;
		this.asString = asString;
		this.noCmdArg = noCmdArg;
	}
	
	public String getName() {
		return name;
	}
	
	public String getHelpText() {
		return helpText;
	}
	
	public String getDefaultValue() {
		return defaultValue;
	}
	
	public String getDefaultValueAsString() {
		if (asString)
			return "\"" + defaultValue + "\"";
		else
			return defaultValue;
	}
	
	public void setDefaultValue(String defaultValue) {
		this.defaultValue = defaultValue;
	}
	
	public boolean excludeFromCmd() {
		return noCmdArg;
	}

}
