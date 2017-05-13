package io.github.lizhangqu.yaml;

import android.support.annotation.Keep;

/**
 * @version V1.0
 * @author: lizhangqu
 * @date: 2017-05-13 19:23
 */
@Keep
public class Yaml {
    static {
        System.loadLibrary("yaml");
    }

    public static final native String list(String value);
}
