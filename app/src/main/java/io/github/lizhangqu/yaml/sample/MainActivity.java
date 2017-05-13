package io.github.lizhangqu.yaml.sample;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import io.github.lizhangqu.yaml.Yaml;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        findViewById(R.id.btn).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String testList = Yaml.list("哈哈哈");
                Log.e("TAG", "testList: " + testList);
            }
        });
    }
}
