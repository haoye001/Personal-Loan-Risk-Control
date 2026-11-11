plugins {
    alias(libs.plugins.android.application)
}

android {
    namespace = "com.example.loanapp"
    compileSdk = 36

    val apiUrl = providers.gradleProperty("loanApiUrl").orElse("https://120.55.39.172").get()
    require(Regex("https?://[A-Za-z0-9.:-]+/?").matches(apiUrl)) {
        "loanApiUrl must be an HTTP(S) server origin"
    }
    buildFeatures {
        buildConfig = true
    }

    defaultConfig {
        buildConfigField("String", "LOAN_API_URL", "\"$apiUrl\"")
        applicationId = "com.example.loanapp"
        minSdk = 24
        targetSdk = 36
        versionCode = 1
        versionName = "1.0"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
    }

    buildTypes {
        release {
            optimization {
                enable = false
            }
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }
}

dependencies {
    implementation(libs.androidx.activity.ktx)
    implementation(libs.androidx.appcompat)
    implementation(libs.androidx.constraintlayout)
    implementation(libs.androidx.core.ktx)
    implementation(libs.androidx.material3)
    implementation(libs.androidx.recyclerview)
    implementation(libs.okhttp)
    implementation(libs.gson)
    implementation(libs.material)
    testImplementation(libs.junit)
    androidTestImplementation(libs.androidx.espresso.core)
    androidTestImplementation(libs.androidx.junit)
}