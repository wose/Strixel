use anyhow::{Context, Result};
use futures::executor::block_on;
use imgui::*;

use winit::{
    dpi::LogicalSize,
    event_loop::EventLoop,
    window::Window,
};

fn main() -> Result<()> {
    wgpu_subscriber::initialize_default_subscriber(None);

    let event_loop = EventLoop::new();
    let mut hidpi_factor = 1.0;
    let instance = wgpu::Instance::new(wgpu::BackendBit::PRIMARY);


    let (window, mut size, surface) = {
        let version = env!("CARGO_PKG_VERSION");
        let window = Window::new(&event_loop).context("Failed to create window")?;
        window.set_inner_size(LogicalSize {
            width: 1280.0,
            height: 720.0,
        });

        window.set_title(&format!("Strixel {}", version));
        let size = window.inner_size();
        let surface = unsafe { instance.create_surface(&window) };

        (window, size, surface)
    };

    let adapter = block_on(instance.request_adapter(&wgpu::RequestAdapterOptions {
        power_preference: wgpu::PowerPreference::HighPerformance,
        compatible_surface: Some(&surface),
    })).context("Failed to get adapter")?;

    let (device, mut queue) = block_on(adapter.request_device(
        &wgpu::DeviceDescriptor {
            features: wgpu::Features::empty(),
            limits: wgpu::Limits::default(),
            shader_validation: false,
        },
        None,
    )).context("Failed to get device")?;

    let mut swap_chain_descriptor = wgpu::SwapChainDescriptor {
        usage: wgpu::TextureUsage::OUTPUT_ATTACHMENT,
        format: wgpu::TextureFormat::Bgra8Unorm,
        width: size.width as u32,
        height: size.height as u32,
        present_mode: wgpu::PresentMode::Mailbox,
    };

    let swap_chain = device.create_swap_chain(&surface, &swap_chain_descriptor);

    let mut imgui = imgui::Context::create();
    let mut platform = imgui_winit_support::WinitPlatform::init(&mut imgui);
    platform.attach_window(imgui.io_mut(), &window, imgui_winit_support::HiDpiMode::Default);
    imgui.set_ini_filename(None);

    let font_size = (13.0 * hidpi_factor) as f32;
    imgui.io_mut().font_global_scale = (1.0 / hidpi_factor) as f32;

    imgui.fonts().add_font(&[FontSource::DefaultFontData {
        config: Some(imgui::FontConfig {
            oversample_h: 1,
            pixel_snap_h: true,
            size_pixels: font_size,
            ..Default::default()
        }),
    }]);



    Ok(())
}
