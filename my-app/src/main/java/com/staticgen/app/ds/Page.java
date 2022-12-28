package com.staticgen.app.ds;

import java.util.HashMap;
import java.util.Map;
import java.util.Properties;

/**
 * An immutable ADT representing the contents of a page.
 */
public class Page {

    private final String name;
    private final Template template;
    private final Map<String, String> contents;

    public Page(Properties props) {
        this.name = props.getProperty("name");
        if (name == null) {
            throw new IllegalStateException("Unnamed page");
        }
        String template = props.getProperty("template");
        if (template == null) {
            throw new IllegalStateException("Undefined template");
        }
        this.template = TemplateMgmt.get(template);
        this.contents = new HashMap<>();
        for (String k : props.stringPropertyNames()) {
            if (!k.equals("name") && !k.equals("template"))
                contents.put(k, props.getProperty(k));
        }
    }

    public String getContent(String key) {
        if (!contents.containsKey(key)) {
            throw new IllegalArgumentException("Invalid content key");
        }
        return contents.get(key);
    }

    public String getName() {
        return name;
    }

    public Template getTemplate() {
        return template;
    }
}
