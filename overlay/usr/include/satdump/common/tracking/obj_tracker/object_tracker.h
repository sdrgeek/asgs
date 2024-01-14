#pragma once

#include <vector>
#include <string>
#include <mutex>
#include "libs/predict/predict.h"
#include <thread>
#include "common/tracking/rotator/rotator_handler.h"
#include "common/geodetic/geodetic_coordinates.h"
#include "common/image/image.h"

namespace satdump
{
    class ObjectTracker
    {
    public:
        enum TrackingMode
        {
            TRACKING_SATELLITE,
            TRACKING_HORIZONS,
            TRACKING_NONE,
        };

        struct SatAzEl
        {
            float az = 0;
            float el = 0;
            NLOHMANN_DEFINE_TYPE_INTRUSIVE(SatAzEl, az, el);
        };

    private: // QTH Config
        double qth_lon = 0;
        double qth_lat = 0;
        double qth_alt = 0;

    private: // General configs/utils
        TrackingMode tracking_mode = TRACKING_NONE;
        bool has_tle = false, is_gui;
        std::mutex general_mutex;

        inline float az_el_to_plot_x(float plot_size, float radius, float az, float el) { return sin(az * DEG_TO_RAD) * plot_size * radius * ((90.0 - el) / 90.0); }
        inline float az_el_to_plot_y(float plot_size, float radius, float az, float el) { return cos(az * DEG_TO_RAD) * plot_size * radius * ((90.0 - el) / 90.0); }

    private: // Satellite Tracking (libpredict)
        std::vector<std::string> satoptions;
        int current_satellite_id = 0;
        std::string satellite_searchstr;

        predict_orbital_elements_t *satellite_object = nullptr;
        predict_position satellite_orbit;
        predict_observer_t *satellite_observer_station;
        predict_observation satellite_observation_pos;

    private: // Horizons Tracking (horizons)
        struct HorizonsV
        {
            double timestamp;
            float az, el;
        };

        std::vector<std::pair<int, std::string>> horizonsoptions = {{-234, "STEREO-A"}};
        int current_horizons_id = 0;
        std::vector<HorizonsV> horizons_data;
        std::string horizons_searchstr;

        double last_horizons_fetch_time = 0;

        std::vector<std::pair<int, std::string>> pullHorizonsList();
        void loadHorizons(double curr_time);
        std::vector<HorizonsV> pullHorizonsData(double start_time, double stop_time, int num_points);

    private: // Internal threads
        bool backend_should_run = true;
        std::thread backend_thread;
        void backend_run();
        bool backend_needs_update = true;

        bool rotatorth_should_run = true;
        std::thread rotatorth_thread;
        void rotatorth_run();

    private: // Current satellite
        SatAzEl sat_current_pos;

        std::mutex upcoming_passes_mtx;
        std::vector<SatAzEl> upcoming_pass_points;
        void updateNextPass(double current_time);

        double next_aos_time = -1, next_los_time = -1;
        SatAzEl sat_next_aos_pos;

    private: // Rotator control
        bool rotator_engaged = false;
        bool rotator_tracking = false;
        SatAzEl rot_current_pos;
        SatAzEl rot_current_req_pos;
        SatAzEl rot_current_reqlast_pos;

        std::mutex rotator_handler_mtx;
        std::shared_ptr<rotator::RotatorHandler> rotator_handler;

        double rotator_update_period = 1;
        bool rotator_park_while_idle = false;
        SatAzEl rotator_park_position;
        double rotator_unpark_at_minus = 60;

    public: // Functions
        nlohmann::json getStatus();
        image::Image<uint8_t> getPolarPlotImg(int size = 256);

        void setQTH(double qth_lon, double qth_lat, double qth_alt);
        void setObject(TrackingMode mode, int objid);
        void setRotator(std::shared_ptr<rotator::RotatorHandler> rot);
        void setRotatorEngaged(bool v);
        void setRotatorTracking(bool v);
        void setRotatorReqPos(float az, float el);

        void renderPolarPlot(bool light_theme = false);
        void renderSelectionMenu();
        void renderObjectStatus();
        void renderRotatorStatus();
        void renderRotatorConfig();

        nlohmann::json getRotatorConfig();
        void setRotatorConfig(nlohmann::json v);

    public:
        ObjectTracker(bool is_gui = false);
        ~ObjectTracker();
    };
}
